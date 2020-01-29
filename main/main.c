#include "hdr.h"
#include "main.h"

#include "../version.c"

//*******************************************************************

uint8_t restart_flag = 0;

uint8_t total_task = 0;
uint8_t last_cmd = 255;

static const char *TAG = "MAIN";
static const char *TAGN = "NVS";
static const char *TAGT = "VFS";
static const char *TAGW = "WIFI";
#ifdef SET_SDCARD
static const char *TAGFAT = "CARD";
#endif
static const char *sdPath = "/sdcard";
static const char *sdConf = "conf.txt";
esp_err_t mntOK = ESP_FAIL;

#ifdef SET_FATDISK
static const char *TAGFATD = "DISK";
static const char *diskPath = "/spiflash";
static const char *diskPart = "disk";
static wl_handle_t s_wl_handle = -1;//WL_INVALID_HANDLE;
static size_t fatfs_size = 0;
esp_err_t diskOK = ESP_FAIL;
#endif

#if defined(SET_SDCARD) || defined(SET_FATDISK)
    char *txtConf = NULL;
#endif

static const char *wmode_name[] = {"NULL", "STA", "AP", "APSTA", "MAX"};
static uint8_t wmode = WIFI_MODE_STA;
static unsigned char wifi_param_ready = 0;
char work_ssid[wifi_param_len] = {0};
static char work_pass[wifi_param_len] = {0};
#ifdef SET_SNTP
    char work_sntp[sntp_srv_len] = {0};
    char time_zone[sntp_srv_len] = {0};
#endif
EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
xSemaphoreHandle prn_mutex;


uint8_t *macs = NULL;

static char sta_mac[18] = {0};
uint32_t cli_id = 0;
char localip[32] = {0};
esp_ip4_addr_t ip4;
ip_addr_t bca;

esp_err_t fs_ok = ESP_FAIL;;

uint32_t tls_client_ip = 0;

#ifdef SET_FTP_CLI
    char ftp_srv_addr[ftp_pole_len] = {0};
    uint16_t ftp_srv_port;
    char ftp_srv_login[ftp_pole_len] = {0};
    char ftp_srv_passwd[ftp_pole_len] = {0};
    bool to_sd = false;
#endif

#ifdef UDP_SEND_BCAST
    static const char *TAGU = "UDP";
    uint8_t udp_start = 0;
    int8_t udp_flag = 0;
#endif

#ifdef SET_SNTP
    uint8_t sntp_go = 0;
#endif

uint16_t tls_port = 0;
#ifdef SET_WEB
uint16_t web_port = 0;
#endif
#ifdef SET_WS
uint16_t ws_port = 0;
#endif

static int s_retry_num = 0;

static uint32_t varta = 0;
static bool scr_ini_done = false;
//static uint8_t scr_len = 0;
//static char scr_line[32] = {0};


#ifdef SET_NET_LOG
    uint16_t net_log_port = 8008;
#endif

#ifdef SET_FMB630
    s_conf gps_ini;
    uint8_t emul_start = 0;
    xSemaphoreHandle rec_mutex;
    xSemaphoreHandle mirror_mutex;
#endif

#ifdef SET_ISR_PIN
    static xQueueHandle q_rst = NULL;

    static void IRAM_ATTR gpio_isr_rst(void *arg)
    {
        uint8_t byte = 1;
        xQueueSendFromISR(q_rst, &byte, NULL);
    }
#endif

//***************************************************************************************************************

esp_err_t read_param(char *param_name, void *param_data, size_t len);
esp_err_t save_param(char *param_name, void *param_data, size_t len);

//***************************************************************************************************************

//--------------------------------------------------------------------------------------
static uint32_t get_varta()
{
    return varta;
}
//--------------------------------------------------------------------------------------
static void periodic_timer_callback(void* arg)
{
    varta++; //100ms period
}
//--------------------------------------------------------------------------------------
uint32_t get_tmr(uint32_t tm)
{
    return (get_varta() + tm);
}
//--------------------------------------------------------------------------------------
int check_tmr(uint32_t tm)
{
    return (get_varta() >= tm ? 1 : 0);
}
//--------------------------------------------------------------------------------------
void print_msg(uint8_t with, const char *tag, const char *fmt, ...)
{
size_t len = BUF_SIZE;//1024

    char *st = (char *)calloc(1, len);
    if (st) {
        if (xSemaphoreTake(prn_mutex, portMAX_DELAY) == pdTRUE) {
            int dl = 0, sz;
            va_list args;
            if (with) {
                struct tm *ctimka;
                time_t it_ct = time(NULL);
                ctimka = localtime(&it_ct);
                dl = sprintf(st, "%02d.%02d %02d:%02d:%02d ",
                                 ctimka->tm_mday,ctimka->tm_mon + 1,ctimka->tm_hour,ctimka->tm_min,ctimka->tm_sec);
            }
            if (tag) dl += sprintf(st+strlen(st), "[%s] ", tag);
            sz = dl;
            va_start(args, fmt);
            sz += vsnprintf(st + dl, len - dl, fmt, args);
            printf(st);
            va_end(args);
#ifdef SET_NET_LOG
            if (tcpCli >= 0) putMsg(st);
#endif
            xSemaphoreGive(prn_mutex);
        }
        free(st);
    }
}
//------------------------------------------------------------------------------------------------------------
const char *wifi_auth_type(wifi_auth_mode_t m)
{

    switch (m) {
        case WIFI_AUTH_OPEN:// = 0
            return "AUTH_OPEN";
        case WIFI_AUTH_WEP:
            return "AUTH_WEP";
        case WIFI_AUTH_WPA_PSK:
            return "AUTH_WPA_PSK";
        case WIFI_AUTH_WPA2_PSK:
            return "AUTH_WPA2_PSK";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "AUTH_WPA_WPA2_PSK";
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "AUTH_WPA2_ENTERPRISE";
        default:
            return "AUTH_UNKNOWN";
    }

}
//------------------------------------------------------------------------------------------------------------
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START :
                esp_wifi_connect();
            break;
            case WIFI_EVENT_STA_DISCONNECTED :
                if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
                    esp_wifi_connect();

                    xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);

//                    s_retry_num++;
                    ets_printf("[%s] retry to connect to the AP\n", TAGW);
                } else ets_printf("[%s] connect to the AP fail\n", TAGW);
            break;
            case WIFI_EVENT_STA_CONNECTED :
            {
                wifi_ap_record_t wd;
                if (!esp_wifi_sta_get_ap_info(&wd)) {
                    ets_printf("[%s] Connected to AP '%s' auth(%u):'%s' chan:%u rssi:%d\n",
                               TAGW,
                               (char *)wd.ssid,
                               (uint8_t)wd.authmode, wifi_auth_type(wd.authmode),
                               wd.primary, wd.rssi);
                }
            }
            break;
        }
    } else if (event_base == IP_EVENT) {
        switch (event_id) {
            case IP_EVENT_STA_GOT_IP :
            {
                ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
                sprintf(localip, IPSTR, IP2STR(&event->ip_info.ip));
                ip4 = event->ip_info.ip;
                bca.u_addr.ip4.addr = ip4.addr | 0xff000000;
                ets_printf("[%s] Local ip_addr : %s\n", TAGW, localip);
                s_retry_num = 0;

                xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
            }
            break;
        };
    }

}
//------------------------------------------------------------------------------------------------------------
bool check_pin(uint8_t pin_num)
{
    gpio_pad_select_gpio(pin_num);
    gpio_pad_pullup(pin_num);
    if (gpio_set_direction(pin_num, GPIO_MODE_INPUT) != ESP_OK) return true;

    return (bool)(gpio_get_level(pin_num));
}
//------------------------------------------------------------------------------------------------------------
#ifdef UDP_SEND_BCAST
#define max_line 256
void udp_task(void *par)
{
udp_start = 1;
err_t err = ERR_OK;
u16_t len, port = 8004;
struct netconn *conn = NULL;
struct netbuf *buf = NULL;
void *data = NULL;
uint32_t cnt = 1;
uint32_t tmr_sec;
char line[max_line] = {0};
bool loop = true;

    total_task++;

    ets_printf("[%s] BROADCAST task started | FreeMem %u\n", TAGU, xPortGetFreeHeapSize());

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);


    while (loop && !restart_flag) {

        if (!udp_flag) { loop = false; break; }

        conn = netconn_new(NETCONN_UDP);
        if (!conn) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            loop = false;
        }

        tmr_sec = get_tmr(_1s);

        while (loop) {
            if (!udp_flag) { loop = false; break; }
            if (check_tmr(tmr_sec)) {
                buf = netbuf_new();
                if (buf) {
                    len = sprintf(line,"{\"DevID\":\"%08X\",\"Time\":%u,\"FreeMem\":%u,\"Ip\":\"%s\",\"To\":\"%s\",\"Pack\":%u}\r\n",
                                        cli_id,
                                        (uint32_t)time(NULL),
                                        xPortGetFreeHeapSize(),
                                        localip, ip4addr_ntoa(&bca.u_addr.ip4), cnt);
                    data = netbuf_alloc(buf, len);
                    if (data) {
                        memset((char *)data, 0, len);
                        memcpy((char *)data, line, len);
                        err = netconn_sendto(conn, buf, (const ip_addr_t *)&bca, port);
                        if (err != ERR_OK) {
                            ESP_LOGE(TAGU,"Sending '%.*s' error=%d '%s'", len, (char *)data, (int)err, lwip_strerr(err));
                        } else {
                            print_msg(1, TAGU, "%s", line);
                            cnt++;
                        }
                    }
                    netbuf_delete(buf); buf = NULL;
                }
                tmr_sec = get_tmr(_10s);//10 sec
            } else vTaskDelay(50 / portTICK_PERIOD_MS);
            if (restart_flag) { loop = false; break; }
        }

        if (conn) { netconn_delete(conn); conn = NULL; }

    }

    udp_start = 0;
    if (total_task) total_task--;
    ets_printf("[%s] BROADCAST task stoped | FreeMem %u\n", TAGU, xPortGetFreeHeapSize());

    vTaskDelete(NULL);
}
#endif
//--------------------------------------------------------------------------------------------------
void initialize_wifi(wifi_mode_t w_mode)
{

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

//    tcpip_adapter_set_hostname(TCPIP_ADAPTER_IF_STA, "esp32.dev");

    ESP_ERROR_CHECK(esp_event_loop_create_default());
         if (w_mode == WIFI_MODE_STA) esp_netif_create_default_wifi_sta();
    else if (w_mode == WIFI_MODE_AP) esp_netif_create_default_wifi_ap();
    else {
        ets_printf("[%s] unknown wi-fi mode - %d | FreeMem %u\n", TAGW, w_mode, xPortGetFreeHeapSize());
        return;
    }


    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(w_mode));


    switch ((uint8_t)w_mode) {
        case WIFI_MODE_STA :
        {
           ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,    &event_handler, NULL));
           ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT,   IP_EVENT_STA_GOT_IP, &event_handler, NULL));

            wifi_config_t wifi_config;
            memset((uint8_t *)&wifi_config, 0, sizeof(wifi_config_t));
            if (wifi_param_ready) {
                memcpy(wifi_config.sta.ssid, work_ssid, strlen(work_ssid));
                memcpy(wifi_config.sta.password, work_pass, strlen(work_pass));
                ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
                ets_printf("[%s] WIFI_MODE - STA: '%s':'%s'\n",
                           TAGW, wifi_config.sta.ssid, wifi_config.sta.password);
            }
            ESP_ERROR_CHECK(esp_wifi_start());
        }
        break;
        case WIFI_MODE_AP:
        {
            ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

            wifi_config_t wifi_configap;
            memset((uint8_t *)&wifi_configap, 0, sizeof(wifi_config_t));
            wifi_configap.ap.ssid_len = strlen(sta_mac);
            memcpy(wifi_configap.ap.ssid, sta_mac, wifi_configap.ap.ssid_len);
            memcpy(wifi_configap.ap.password, work_pass, strlen(work_pass));
            wifi_configap.ap.channel = 6;
            wifi_configap.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;//WIFI_AUTH_WPA_WPA2_PSK
            wifi_configap.ap.ssid_hidden = 0;
            wifi_configap.ap.max_connection = 4;
            wifi_configap.ap.beacon_interval = 100;
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_configap));
            ESP_ERROR_CHECK(esp_wifi_start());
            ets_printf("[%s] WIFI_MODE - AP: '%s':'%s'\n", TAGW, wifi_configap.ap.ssid, work_pass);
        }
        break;
    }
}
//********************************************************************************************************************
esp_err_t read_param(char *param_name, void *param_data, size_t len)//, int8_t type)
{
nvs_handle mhd;

    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &mhd);
    if (err != ESP_OK) {
        ESP_LOGE(TAGN, "%s(%s): Error open '%s'", __func__, param_name, STORAGE_NAMESPACE);
    } else {//OK
        err = nvs_get_blob(mhd, param_name, param_data, &len);
        if (err != ESP_OK) {
            ESP_LOGE(TAGN, "%s: Error read '%s' from '%s'", __func__, param_name, STORAGE_NAMESPACE);
        }
        nvs_close(mhd);
    }

    return err;
}
//--------------------------------------------------------------------------------------------------
esp_err_t save_param(char *param_name, void *param_data, size_t len)
{
nvs_handle mhd;

    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &mhd);
    if (err != ESP_OK) {
        ESP_LOGE(TAGN, "%s(%s): Error open '%s'", __func__, param_name, STORAGE_NAMESPACE);
    } else {
        err = nvs_set_blob(mhd, param_name, (uint8_t *)param_data, len);
        if (err != ESP_OK) {
            ESP_LOGE(TAGN, "%s: Error save '%s' with len %u to '%s'", __func__, param_name, len, STORAGE_NAMESPACE);
        } else err = nvs_commit(mhd);
        nvs_close(mhd);
    }

    return err;
}
//--------------------------------------------------------------------------------------------------
uint32_t get_vcc()
{
    return ((uint32_t)(adc1_get_raw(ADC1_TEST_CHANNEL) * 0.8));
}
//--------------------------------------------------------------------------------------------------
float get_tChip()
{
    return (((temprature_sens_read() - 40) - 32) * 5/9);
}
//------------------------------------------------------------------------------------------------------------
#if defined(SET_SDCARD) || defined(SET_FATDISK)
static int get_file_size(const char *full_filename)
{
    struct stat sta;
    stat(full_filename, &sta);

    return sta.st_size;
}
//---------------------------------------------------
static bool print_dir(const char *tag, const char *dir_name)
{
bool ret = false;

    print_msg(1, tag, "Open DIR '%s':\n", dir_name);
    struct dirent *de = NULL;
    DIR *dir = opendir(dir_name);
    if (dir) {
        char *tmp = (char *)calloc(1, 384);
        if (tmp) {
            uint32_t kol = 0;
            while ( (de = readdir(dir)) != NULL ) {
                if (strcasestr(de->d_name, sdConf)) ret = true;//conf.txt present !!!
                sprintf(tmp, "%s/%s", dir_name, de->d_name);
                print_msg(0, NULL, "\tfile: type=%d name='%s' size=%d\n", de->d_type, de->d_name, get_file_size(tmp));
                seekdir(dir, ++kol);
            }
            free(tmp);
        }
    } else print_msg(1, tag, "Open DIR '%s' Error | FreeMem %u", dir_name, xPortGetFreeHeapSize());

    return ret;
}
//---------------------------------------------------
static int wrFile(esp_err_t mnt, const char *tag, const char *full_filename, const char *buf, int len)
{
int ret = -1;

    if (mnt == ESP_OK) {
        FILE *f = fopen(full_filename, "w+");
        if (f) {
            ret = fwrite(buf, 1, len, f);
            fclose(f);
            print_msg(1, tag, "Write file '%s' OK (%d bytes)\n", full_filename, ret);
        } else print_msg(1, tag, "Failed to open file '%s' for writing\n", full_filename);
    }

    return ret;
}
//---------------------------------------------------
static char *rdFile(esp_err_t mnt, const char *tag, const char *full_filename, int *rd)
{
char *ret = NULL;
int rx = 0;

    if (mnt == ESP_OK) {
        int len = get_file_size(full_filename);
        if (len > 0) {
            if (len > len2k - 1) len = len2k - 1;
            char *buf = (char *)calloc(1, len2k);
            if (buf) {
                FILE *f = fopen(full_filename, "r");
                if (f) {
                    rx = fread(buf, 1, len, f);
                    fclose(f);
                    ret = buf;
                    if (rx != len) print_msg(1, tag, "Read file '%s' Error (%d)\n", full_filename, rx);
                              else print_msg(1, tag, "Read file '%s' OK (%d bytes)\n", full_filename, rx);
                } else {
                    free(buf);
                    print_msg(1, tag, "Failed to open file '%s' for reading\n", full_filename);
                }
            }
        }
    }

    *rd = rx;
    return ret;
}
#endif
//************************************************************************************************************
void app_main()
{

    total_task = 0;
    char line[288];

    vTaskDelay(1500 / portTICK_RATE_MS);

    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAGN, "1: nvs_flash_init() ERROR (0x%x) !!!", err);
        nvs_flash_erase();
        err = nvs_flash_init();
        if (err != ESP_OK) {
            ESP_LOGE(TAGN, "2: nvs_flash_init() ERROR (0x%x) !!!", err);
            while (1);
        }
    }

    vTaskDelay(1000 / portTICK_RATE_MS);

    macs = (uint8_t *)calloc(1, 6);
    if (macs) {
        esp_efuse_mac_get_default(macs);
        sprintf(sta_mac, MACSTR, MAC2STR(macs));
        memcpy(&cli_id, &macs[2], 4);
        cli_id = ntohl(cli_id);
    }

    ets_printf("\nApp version %s | MAC %s | SDK Version %s | FreeMem %u\n", Version, sta_mac, esp_get_idf_version(), xPortGetFreeHeapSize());

    //--------------------------------------------------------

    esp_log_level_set("wifi", ESP_LOG_WARN);

    //--------------------------------------------------------

    gpio_pad_select_gpio(GPIO_LOG_PIN);//white LED
    gpio_pad_pullup(GPIO_LOG_PIN);
    gpio_set_direction(GPIO_LOG_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_LOG_PIN, LED_OFF);
    //
    gpio_pad_select_gpio(GPIO_GPS_PIN);//green LED
    gpio_pad_pullup(GPIO_GPS_PIN);
    gpio_set_direction(GPIO_GPS_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_GPS_PIN, LED_OFF);
    //

    //--------------------------------------------------------

    bool rt = check_pin(GPIO_WIFI_PIN);//pin17
    if (!rt) ets_printf("[%s] === CHECK_WIFI_REWRITE_PIN %d LEVEL IS %d ===\n", TAGT, GPIO_WIFI_PIN, rt);

    //--------------------------------------------------------

    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        /* name is optional, but may help identify the timer when debugging */
        .name = "periodic"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 100000));//100000 us = 100 ms
    ets_printf("[%s] Started timer with period 100 ms, time since boot: %lld/%llu\n",
               TAGT, esp_timer_get_time(), get_varta() * 100);

    //--------------------------------------------------------

    //CLI_ID
    err = read_param(PARAM_CLIID_NAME, (void *)&cli_id, sizeof(uint32_t));
    if (err != ESP_OK) save_param(PARAM_CLIID_NAME, (void *)&cli_id, sizeof(uint32_t));
    ets_printf("[%s] DEVICE_ID='%08X'\n", TAGT, cli_id);

#ifdef SET_SNTP
    //SNTP + TIME_ZONE
    memset(work_sntp, 0, sntp_srv_len);
    err = read_param(PARAM_SNTP_NAME, (void *)work_sntp, sntp_srv_len);
    if (err != ESP_OK) {
        memset(work_sntp, 0, sntp_srv_len);
        memcpy(work_sntp, sntp_server_def, strlen((char *)sntp_server_def));
        save_param(PARAM_SNTP_NAME, (void *)work_sntp, sntp_srv_len);
    }
    memset(time_zone, 0, sntp_srv_len);
    err = read_param(PARAM_TZONE_NAME, (void *)time_zone, sntp_srv_len);
    if (err != ESP_OK) {
        memset(time_zone, 0, sntp_srv_len);
        memcpy(time_zone, sntp_tzone_def, strlen((char *)sntp_tzone_def));
        save_param(PARAM_TZONE_NAME, (void *)time_zone, sntp_srv_len);
    }
    ets_printf("[%s] SNTP_SERVER '%s' TIME_ZONE '%s'\n", TAGT, work_sntp, time_zone);
#endif

    //WIFI
    //   MODE
    bool rt0 = check_pin(GPIO_WMODE_PIN);//pin16
    if (!rt0) ets_printf("[%s] === CHECK_WIFI_MODE_PIN %d LEVEL IS %d ===\n", TAGT, GPIO_WMODE_PIN, rt0);


    uint8_t wtmp;
    err = read_param(PARAM_WMODE_NAME, (void *)&wtmp, sizeof(uint8_t));
    if (rt0) {//set wifi_mode from flash
        if (err == ESP_OK) {
            wmode = wtmp;
        } else {
            wmode = WIFI_MODE_AP;
            save_param(PARAM_WMODE_NAME, (void *)&wmode, sizeof(uint8_t));
        }
    } else {//set AP wifi_mode
        wmode = WIFI_MODE_AP;
        if (err == ESP_OK) {
            if (wtmp != wmode) save_param(PARAM_WMODE_NAME, (void *)&wmode, sizeof(uint8_t));
        } else {
            save_param(PARAM_WMODE_NAME, (void *)&wmode, sizeof(uint8_t));
        }
    }
// Set STA mode !!!
//    wmode = WIFI_MODE_STA;
//    save_param(PARAM_WMODE_NAME, (void *)&wmode, sizeof(uint8_t));
/**/
    ets_printf("[%s] WIFI_MODE (%d): %s\n", TAGT, wmode, wmode_name[wmode]);

#ifdef UDP_SEND_BCAST
    if (wmode == WIFI_MODE_STA) udp_flag = 1;
#endif

    //   SSID
    memset(work_ssid, 0, wifi_param_len);
    err = read_param(PARAM_SSID_NAME, (void *)work_ssid, wifi_param_len);
    if ((err != ESP_OK) || (!rt)) {
        memset(work_ssid,0,wifi_param_len);
        memcpy(work_ssid, EXAMPLE_WIFI_SSID, strlen(EXAMPLE_WIFI_SSID));
        save_param(PARAM_SSID_NAME, (void *)work_ssid, wifi_param_len);
    }
    //   KEY
    memset(work_pass, 0, wifi_param_len);
    err = read_param(PARAM_KEY_NAME, (void *)work_pass, wifi_param_len);
    if ((err != ESP_OK) || (!rt)) {
        memset(work_pass,0,wifi_param_len);
        memcpy(work_pass, EXAMPLE_WIFI_PASS, strlen(EXAMPLE_WIFI_PASS));
        save_param(PARAM_KEY_NAME, (void *)work_pass, wifi_param_len);
    }
    ets_printf("[%s] WIFI_STA_PARAM: '%s:%s'\n", TAGT, work_ssid, work_pass);

    wifi_param_ready = 1;

#ifdef SET_TLS_SRV
    err = read_param(PARAM_TLS_PORT, (void *)&tls_port, sizeof(uint16_t));
    if ((err != ESP_OK) || (!rt)) {
        tls_port = TLS_PORT_DEF;
        save_param(PARAM_TLS_PORT, (void *)&tls_port, sizeof(uint16_t));
    }
    ets_printf("[%s] TLS_PORT: %u\n", TAGT, tls_port);
#endif


#ifdef SET_FTP_CLI
    //   ftp_srv_addr
    memset(ftp_srv_addr, 0, sizeof(ftp_srv_addr));
    err = read_param(PARAM_FTP_SRV_ADDR_NAME, (void *)ftp_srv_addr, sizeof(ftp_srv_addr));
    if (err != ESP_OK) {
        memset(ftp_srv_addr, 0, sizeof(ftp_srv_addr));
        strncpy(ftp_srv_addr, FTP_SRV_ADDR_DEF, ftp_pole_len);
        save_param(PARAM_FTP_SRV_ADDR_NAME, (void *)ftp_srv_addr, sizeof(ftp_srv_addr));
    }
    //   ftp_srv_port
    ftp_srv_port = FTP_SRV_PORT_DEF;
    err = read_param(PARAM_FTP_SRV_PORT_NAME, (void *)&ftp_srv_port, sizeof(uint16_t));
    if (err != ESP_OK) {
        ftp_srv_port = FTP_SRV_PORT_DEF;
        save_param(PARAM_FTP_SRV_PORT_NAME, (void *)&ftp_srv_port, sizeof(uint16_t));
    }
    //   ftp_srv_login
    memset(ftp_srv_login, 0, sizeof(ftp_srv_login));
    err = read_param(PARAM_FTP_SRV_LOGIN_NAME, (void *)ftp_srv_login, sizeof(ftp_srv_login));
    if (err != ESP_OK) {
        memset(ftp_srv_login, 0, sizeof(ftp_srv_login));
        strncpy(ftp_srv_login, FTP_SRV_LOGIN_DEF, ftp_pole_len);
        save_param(PARAM_FTP_SRV_LOGIN_NAME, (void *)ftp_srv_login, sizeof(ftp_srv_login));
    }
    //   ftp_srv_passwd
    memset(ftp_srv_passwd, 0, sizeof(ftp_srv_passwd));
    err = read_param(PARAM_FTP_SRV_PASSWD_NAME, (void *)ftp_srv_passwd, sizeof(ftp_srv_passwd));
    if (err != ESP_OK) {
        memset(ftp_srv_passwd, 0, sizeof(ftp_srv_passwd));
        strncpy(ftp_srv_passwd, FTP_SRV_PASSWD_DEF, ftp_pole_len);
        save_param(PARAM_FTP_SRV_PASSWD_NAME, (void *)ftp_srv_passwd, sizeof(ftp_srv_passwd));
    }
    ets_printf("[%s] FTP SERVER : '%s:%u' (%s/%s)\n",
               TAGT,
               ftp_srv_addr, ftp_srv_port,
               ftp_srv_login, ftp_srv_passwd);
#endif

//******************************************************************************************************
//ADC_ATTEN_0db   = 0,  /*!<The input voltage of ADC will be reduced to about 1/1 */
//ADC_ATTEN_2_5db = 1,  /*!<The input voltage of ADC will be reduced to about 1/1.34 */
//ADC_ATTEN_6db   = 2,  /*!<The input voltage of ADC will be reduced to about 1/2 */
//ADC_ATTEN_11db  = 3,  /*!<The input voltage of ADC will be reduced to about 1/3.6*/
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_11db);

    prn_mutex = xSemaphoreCreateMutex();

//******************************************************************************************************

    initialize_wifi(wmode);
    vTaskDelay(1000 / portTICK_RATE_MS);

//******************************************************************************************************

#ifdef SET_SSD1306
    lcd_mutex = xSemaphoreCreateMutex();

    i2c_ssd1306_init();

    ssd1306_on(false);
    vTaskDelay(500 / portTICK_RATE_MS);

    esp_err_t ssd_ok = ssd1306_init();
    if (ssd_ok == ESP_OK) ssd1306_pattern();
    //ssd1306_invert();
    ssd1306_clear();

    scr_ini_done = true;
    //
    char stk[128] = {0};
    struct tm *dtimka;
    int tu, tn;
    time_t dit_ct;
    uint32_t adc_tw = get_tmr(0);
#endif


#ifdef SET_SNTP
    if (wmode & 1) {// WIFI_MODE_STA) || WIFI_MODE_APSTA
        if (xTaskCreatePinnedToCore(&sntp_task, "sntp_task", STACK_SIZE_2K, work_sntp, 10, NULL, 0) != pdPASS) {//5,NULL,1
            ESP_LOGE(TAGS, "Create sntp_task failed | FreeMem %u", xPortGetFreeHeapSize());
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
#endif


#ifdef SET_NET_LOG
    msgq = xQueueCreate(8, sizeof(s_net_msg));//create msg queue

    if (xTaskCreatePinnedToCore(&net_log_task, "net_log_task", 4*STACK_SIZE_1K, &net_log_port, 6, NULL, 1) != pdPASS) {//7,NULL,1
        ESP_LOGE(TAGS, "Create net_log_task failed | FreeMem %u", xPortGetFreeHeapSize());
    }
    vTaskDelay(1000 / portTICK_RATE_MS);
#endif


//*****************************************************************************************************

#ifdef SET_SDCARD

    vTaskDelay(1000 / portTICK_RATE_MS);

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();

    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = PIN_NUM_MISO;
    slot_config.gpio_mosi = PIN_NUM_MOSI;
    slot_config.gpio_sck  = PIN_NUM_CLK;
    slot_config.gpio_cs   = PIN_NUM_CS;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 3,
        .allocation_unit_size = 8 * 1024 //16 * 1024
    };

    sdmmc_card_t *card;
    mntOK = esp_vfs_fat_sdmmc_mount(sdPath, &host, &slot_config, &mount_config, &card);

    if (mntOK != ESP_OK) {
        if (mntOK == ESP_FAIL) sprintf(line, "Failed to mount filesystem on sdcard.\n");
                          else sprintf(line, "Failed to initialize the sdcard\n");
    } else sprintf(line, "Mount %s OK | FreeMem %u\n", sdPath, xPortGetFreeHeapSize());
    print_msg(1, TAGFAT, line);

    if (mntOK == ESP_OK) {
        sdmmc_card_print_info(stdout, card);
        //
        sprintf(line, "%s/", sdPath);
        if (print_dir(TAGFAT, line)) {// read /sdcard/conf.txt if file present
            strcat(line, sdConf);
            int rxlen = 0;
            txtConf = rdFile(mntOK, TAGFAT, line, &rxlen);
        } else print_msg(1, TAGFAT, "Configuration file %s%s NOT present | FreeMem %u\n", line, sdConf, xPortGetFreeHeapSize());
    }

#endif
//*****************************************************************************************************
#ifdef SET_FATDISK

    vTaskDelay(1000 / portTICK_RATE_MS);

    const esp_vfs_fat_mount_config_t mount_disk = {
        .max_files = 3,
        .format_if_mount_failed = false
    };

    diskOK = esp_vfs_fat_spiflash_mount(diskPath, diskPart, &mount_disk, &s_wl_handle);
    if (diskOK != ESP_OK) {
        print_msg(1, TAGFATD, "Failed to mount FATFS partition '%s' on device '%s' (0x%x)", diskPart, diskPath, diskOK);
    } else {
        fatfs_size = wl_size(s_wl_handle);
        print_msg(1, TAGFATD, "Mount FATFS partition '%s' on device '%s' OK (size=%u)\n", diskPart, diskPath, fatfs_size);
    }

    if (diskOK == ESP_OK) {
        bool delit = check_pin(GPIO_DELIT_PIN);
        if (!delit) print_msg(1, TAGFATD, "=== GPIO_DELIT_PIN(%d)=%d ===\n", GPIO_DELIT_PIN, delit);

        sprintf(line, "%s/", diskPath);
        bool yes = print_dir(TAGFATD, line);
        strcat(line, sdConf);
        if (!delit) unlink(line);//delete /spiflash/con.txt
        else if (txtConf && !yes) {//write conf.txt to /spiflash
            int rxlen = strlen(txtConf);
            wrFile(diskOK, TAGFATD, line, txtConf, rxlen);

            sprintf(line, "%s/", diskPath);
            print_dir(TAGFATD, line);
        }
    }

    if (txtConf) free(txtConf);

#endif
//*****************************************************************************************************

#ifdef SET_FMB630

    vTaskDelay(1000 / portTICK_RATE_MS);

    rec_mutex    = xSemaphoreCreateMutex();
    mirror_mutex = xSemaphoreCreateMutex();

    memset(&gps_ini, 0, sizeof(s_conf));
    //
    int cfgErr = 1;


    if (mntOK == ESP_OK) {
        sprintf(line, "%s/%s", sdPath, sdConf);
        cfgErr = read_cfg(&gps_ini, line, 0);
        //
        esp_vfs_fat_sdmmc_unmount();
        print_msg(1, TAGFAT, "Unmounted FAT filesystem on '%s' | FreeMem %u\n", sdPath, xPortGetFreeHeapSize());
        mntOK = ESP_FAIL;
    }
    if (cfgErr) {
        if (diskOK == ESP_OK) {
            sprintf(line, "%s/%s", diskPath, sdConf);
            cfgErr = read_cfg(&gps_ini, line, 0);
        }
    }
    if (cfgErr) {
        strcpy(gps_ini.srv, DEF_GPS_ADDR);//char srv[max_url_len];//"server=",//92.53.65.4
        gps_ini.port = DEF_GPS_PORT;//9900;//uint16_t port;//9090
        memcpy(gps_ini.imei, "351580051430040", size_imei);//char imei[size_imei];//"imei=",//351580051430037
        gps_ini.mode = 0;//uint8_t mode;//"mode="//0
        gps_ini.snd_park = 30;//int snd_park;//"period_park="//30
        gps_ini.snd_move = 10;//int snd_move;//"period_move="//10
        gps_ini.wait_ack = 15;//int wait_ack;//"wait_ack="//15
        gps_ini.wait_before_new_connect = 10;//int wait_before_new_connect;//"wait_before_new_connect=",//3
        //Москва Строгино
        float latf = 55.803893;//54.699680;
        float lonf = 37.403016;//20.514002;
        gps_ini.latitude = latf * 10000000;//0x0342A682;//uint32_t latitude;// широта
        gps_ini.longitude = lonf * 10000000;//0x013904D0;//uint32_t longitude;// долгота
    }

    if (xTaskCreatePinnedToCore(&fmb630_task, "fmb630_task", 8*STACK_SIZE_1K, &gps_ini, 9, NULL, 0) != pdPASS) {
        ESP_LOGE(TAGGPS, "Create fmb630_task failed | FreeMem %u", xPortGetFreeHeapSize());
    }
    vTaskDelay(1000 / portTICK_RATE_MS);
#endif


#ifdef SET_FTP_CLI
    uint8_t mdone = 0;
    s_ftp_var farg;
    memset((uint8_t *)&farg, 0, sizeof(s_ftp_var));
    farg.devPort = ftp_srv_port;//FTP_SRV_PORT_DEF;
    strcpy(farg.devSrv, ftp_srv_addr);//FTP_SRV_ADDR_DEF);
    strcpy(farg.devLogin, ftp_srv_login);//FTP_SRV_LOGIN_DEF);
    strcpy(farg.devPasswd, ftp_srv_passwd);//FTP_SRV_PASSWD_DEF);
    strcpy(farg.devConf, sdConf);
    if (to_sd) {
        farg.devMnt = mntOK;
        strcpy(farg.devPath, sdPath);
    } else {
        farg.devMnt = diskOK;
        strcpy(farg.devPath, diskPath);
    }
    print_msg(1, TAGFTP, "Init FTP struct for '%s/%s' doe | FreeMem %u\n", farg.devPath, farg.devConf,  xPortGetFreeHeapSize());
#endif


#ifdef SET_TLS_SRV
    if (xTaskCreatePinnedToCore(&tls_task, "tls_task", 8*STACK_SIZE_1K, &tls_port, 8, NULL, 0) != pdPASS) {//6,NULL,1)
        ESP_LOGE(TAGTLS, "Create tls_task failed | FreeMem %u", xPortGetFreeHeapSize());
    }
    vTaskDelay(500 / portTICK_RATE_MS);
#endif


#ifdef SET_ISR_PIN
    gpio_config_t io_conf = {
        .intr_type    = GPIO_PIN_INTR_POSEDGE,//GPIO_INTR_NEGEDGE,//GPIO_INTR_ANYEDGE,//GPIO_PIN_INTR_POSEDGE,//interrupt of rising edge
        .pull_up_en   = 0,
        .pull_down_en = 1,
        .pin_bit_mask = GPIO_RESTART_PIN_SEL,//bit mask of the pins
        .mode         = GPIO_MODE_INPUT,
    };
    gpio_config(&io_conf);
    uint8_t byte_rst = 0;
    q_rst = xQueueCreate(2, sizeof(uint8_t));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_RESTART_PIN, gpio_isr_rst, NULL);
#else
    check_pin(GPIO_RESTART_PIN);
#endif


    while (!restart_flag) {//main loop

#ifdef SET_SSD1306
        if (check_tmr(adc_tw)) {
            if (ssd_ok == ESP_OK) {
                dit_ct = time(NULL);
                dtimka = localtime(&dit_ct);
                sprintf(stk,"%02d.%02d %02d:%02d:%02d\n",
                                    dtimka->tm_mday, dtimka->tm_mon + 1,
                                    dtimka->tm_hour, dtimka->tm_min, dtimka->tm_sec);
                tu = strlen(localip);
                if ((tu > 0) && (tu <= 16)) {
                    tn = (16 - tu ) / 2;
                    if ((tn > 0) && (tn < 8)) sprintf(stk+strlen(stk),"%*.s",tn," ");
                    sprintf(stk+strlen(stk),"%s", localip);
                }
                ssd1306_text_xy(stk, 2, 1);
            }
            adc_tw = get_tmr(_1s);
        }
#endif

        if (wmode == WIFI_MODE_STA) {// WIFI_MODE_STA || WIFI_MODE_APSTA
#ifdef SET_SNTP
            if (sntp_go) {
                if (!sntp_start) {
                    sntp_go = 0;
                    if (xTaskCreatePinnedToCore(&sntp_task, "sntp_task", STACK_SIZE_2K, work_sntp, 5, NULL, 0)  != pdPASS) {//5//7 core=1
                        ESP_LOGE(TAGS, "Create sntp_task failed | FreeMem %u", xPortGetFreeHeapSize());
                    }
                    vTaskDelay(500 / portTICK_RATE_MS);
                } else vTaskDelay(50 / portTICK_RATE_MS);
            }
#endif

#ifdef UDP_SEND_BCAST
            if ((!udp_start) && (udp_flag == 1)) {
                if (xTaskCreatePinnedToCore(&udp_task, "disk_task", STACK_SIZE_1K5, NULL, 5, NULL, 0) != pdPASS) {//5,NULL,1)
                    ESP_LOGE(TAGU, "Create udp_task failed | FreeMem %u", xPortGetFreeHeapSize());
                }
                vTaskDelay(500 / portTICK_RATE_MS);
            }
#endif
        }

#ifdef SET_FTP_CLI
        if (!mdone) {
            if (!ftp_start) {
#ifdef SET_SDCARD
                if (mntOK == ESP_OK) {
                    esp_vfs_fat_sdmmc_unmount();
                    print_msg(1, TAGFAT, "Unmounted FAT filesystem on '%s' | FreeMem %u\n", sdPath, xPortGetFreeHeapSize());
                    mntOK = ESP_FAIL;
                }
#endif
#ifdef SET_FATDISK
                if (diskOK == ESP_OK) {
                    esp_vfs_fat_spiflash_unmount(diskPath, s_wl_handle);
                    print_msg(1, TAGFATD, "Unmounted FAT filesystem on '%s' | FreeMem %u\n", diskPath, xPortGetFreeHeapSize());
                    diskOK = ESP_FAIL;
                }
#endif
            }
        }
        if (ftp_go_flag) {
            if (!ftp_start) {
                ftp_go_flag = 0;
                if (to_sd) {//to_sd = true -> save to /sdcard
                    if (mntOK != ESP_OK) mntOK = esp_vfs_fat_sdmmc_mount(sdPath, &host, &slot_config, &mount_config, &card);
                    farg.devMnt = mntOK;
                    strcpy(farg.devPath, sdPath);
                } else {//to_sd = false -> save to /spiflash
                    if (diskOK != ESP_OK) diskOK = esp_vfs_fat_spiflash_mount(diskPath, diskPart, &mount_disk, &s_wl_handle);
                    farg.devMnt = diskOK;
                    strcpy(farg.devPath, FTP_PATH_DEF);
                }
                if (farg.devMnt == ESP_OK) {
                    if (xTaskCreatePinnedToCore(&ftp_cli_task, "ftp_cli_task", 8*STACK_SIZE_1K, &farg, 8, NULL, 1) != pdPASS) {
                        ESP_LOGE(TAGFTP, "Error create ftp_cli_task | FreeMem %u", xPortGetFreeHeapSize());
                    } else vTaskDelay(500 / portTICK_RATE_MS);
                }
            }
        }
#endif

#ifdef SET_ISR_PIN
        if (xQueueReceive(q_rst, &byte_rst, 10/portTICK_RATE_MS) == pdTRUE) {
            if (byte_rst) {
                restart_flag = byte_rst;
                break;
            }
        }
#else
        if (gpio_get_level(GPIO_RESTART_PIN)) {
            restart_flag = 1;
            break;
        }
#endif


    }//while (!restart_flag)

#ifdef SET_SSD1306
    ssd1306_clear();
#endif
    print_msg(1, TAG, "!!! RESTART_PIN is PRESSED !!!\n");


    uint8_t cnt = 30;
    print_msg(1, TAG, "Waiting for all task closed...%d sec.\n", cnt/10);
    while (total_task) {
        cnt--; if (!cnt) break;
        vTaskDelay(100 / portTICK_RATE_MS);
    }
    print_msg(1, TAG, "(%d) DONE. Total unclosed task %d\n", cnt, total_task);

    // UMOUNT ALL
#ifdef SET_FATDISK
    if (diskOK == ESP_OK) {
        esp_vfs_fat_spiflash_unmount(diskPath, s_wl_handle);
        print_msg(1, TAGFATD, "Unmounted FAT filesystem partition '%s' | FreeMem %u\n", diskPart, xPortGetFreeHeapSize());
        diskOK = ESP_FAIL;
    }
#endif
#ifdef SET_SDCARD
    if (mntOK == ESP_OK) {
        esp_vfs_fat_sdmmc_unmount();
        print_msg(1, TAGFAT, "Unmounted FAT filesystem on '%s' | FreeMem %u\n", sdPath, xPortGetFreeHeapSize());
        mntOK = ESP_FAIL;
    }
#endif

    if (macs) free(macs);

    ets_printf("[%s] Waiting wifi stop...\n\n", TAG);

    vTaskDelay(2000 / portTICK_RATE_MS);

    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());

    esp_restart();

}
