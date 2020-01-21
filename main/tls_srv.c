#include "hdr.h"

#ifdef SET_TLS_SRV

#include "main.h"

//******************************************************************************************

const char *instr = "alarm";
const char *TAGTLS = "TLS";
uint8_t tls_start = 0;
uint8_t tls_hangup = 0;
char tls_cli_ip_addr[32] = {0};

#define cmd_name_all 11
const char *CmdName[] = {
    "auth",//0
    "udp",//1
    "sntp",//2
    "sntp_srv",//3
    "time_zone",//4
    "restart",//5
    "time",//6
    "ftp_go",//7
    "ftp_srv",//8
    "ftp_user",//9
    "get"//10
};//9
//{"auth":"hash"} - hash md5_from_key_word
//{"udp":"on"} , {"udp":"off"}
//{"sntp":"on"}
//{"sntp_srv":"ip_ntp_server"}
//{"time_zone":"UTC+02:00"}
//{"restart":"on"}
//{"time":"1493714647"} , {"time":1493714647}
//{"ftp_go":"on"}
//{"ftp_srv":"10.100.0.101:9221"}
//{"ftp_user":"test:9999"}
//{"get":"status"}

const char *l_on  = "on";
const char *l_off = "off";

//------------------------------------------------------------------------------------------
char *get_json_str(cJSON *tp)
{
    if (tp->type != cJSON_String) return NULL;

    char *st = cJSON_Print(tp);
    if (st) {
        if (*st == '"') {
            int len = strlen(st);
            memcpy(st, st + 1, len - 1);
            *(st + len - 2) = '\0';
        }
    }

    return st;
}
//------------------------------------------------------------------------------------------
int parser_json_str(const char *st, uint8_t *au, const char *str_md5, uint8_t *rst)
{
int yes = -1;
uint8_t done = 0, ind_c = 255, rcpu = 0;
int k, i, val_bin = -1;
char stz[64];
char *uki = NULL;


    cJSON *obj = cJSON_Parse(st);
    if (obj) {
        cJSON *tmp = NULL;
        char *val = NULL;
        for (i = 0; i < cmd_name_all; i++) {
            tmp = cJSON_GetObjectItem(obj, CmdName[i]);
            if (tmp) {
                ind_c = i;
                switch (tmp->type) {
                    case cJSON_False:
                    case cJSON_True:
                    case cJSON_NULL:
                    break;
                    case cJSON_Number:
                        val_bin = tmp->valueint;
                    break;
                    case cJSON_String:
                        if (val) { free(val); val = NULL; }
                        val = get_json_str(tmp);
                    break;
                    case cJSON_Array://error !
                    break;
                    case cJSON_Object:
                    break;
                }
                done = 0;
                if ((val) || (val_bin != -1)) {
                    switch (ind_c) {
                        case CTRL_AUTH://0://auth
                            if (val) {
                                done = 1;
                                if ((au) && (str_md5)) {
                                    if (!strcmp(val, str_md5)) {
                                        yes = 0;
                                        *au = done;
                                    }
                                }
                            }
                        break;
#ifdef UDP_SEND_BCAST
                        case CTRL_UDP://1://udp
                            if (val) {
                                if (au) {//from tls_client
                                    if (*au) {
                                        if (!strcmp(val, l_on)) {
                                            udp_flag = 1;
                                            yes = 0;
                                        } else if (!strcmp(val, l_off)) {
                                            udp_flag = 0;
                                            yes = 0;
                                        }
                                    }
                                }
                                done = 1;
                            }
                        break;
#endif
#ifdef SET_SNTP
                        case CTRL_SNTP://2://sntp
                            if (val) {
                                if (au) {//from tls_client
                                    if (*au) {
                                        if (!strcmp(val, l_on)) {
                                            sntp_go = 1;
                                            yes = 0;
                                        }
                                    }// else yes=-1;
                                }
                                done = 1;
                            }
                        break;
                        case CTRL_SNTP_SRV://3://sntp_srv
                            if (val) {
                                if (au) {//from tls_client
                                    if (*au) {
                                        k = strlen(val); if (k > sntp_srv_len - 1) k = sntp_srv_len - 1;
                                        memset(work_sntp, 0, sntp_srv_len);
                                        strncpy(work_sntp, val, k);
                                        save_param(PARAM_SNTP_NAME, (void *)work_sntp, sntp_srv_len);
                                        sntp_go = 1;
                                        yes = 0;
                                    }
                                }
                                done = 1;
                            }
                        break;
                        case CTRL_TIME_ZONE://4://time_zone
                            if (val) {
                                if (au) {//from tls_client
                                    if (*au) {
                                        if (strcmp(val, time_zone)) {
                                            k = strlen(val);
                                            if ((k < sntp_srv_len) && (k > 0)) {
                                                memset(time_zone, 0, sntp_srv_len);
                                                strncpy(time_zone, val, k);
                                                save_param(PARAM_TZONE_NAME, (void *)time_zone, sntp_srv_len);
                                                sntp_go = 1;
                                            }
                                        }
                                        yes = 0;
                                    }
                                }
                                done = 1;
                            }
                        break;
#endif
                        case CTRL_RESTART://5://restart
                            if (val) {
                                if (au) {//from tls_client
                                    if (*au) {
                                        if (!strcmp(val, l_on)) {
                                            rcpu = 1;
                                            yes = 0;
                                        }
                                    }
                                }
                                done = 1;
                                if (rst) *rst = rcpu;
                            }
                        break;
                        case CTRL_TIME://6://time
                            k = -1;
                            if (val) k = atoi(val); else if (val_bin != -1) k = val_bin;
                            if (k > 0) {
                                if (au) {
                                    if (*au) {
                                        SNTP_SET_SYSTEM_TIME_US( (time_t)k, 0 );
                                        yes = 0;
                                    }
                                }
                            }
                            done = 1;
                        break;
#ifdef SET_FTP_CLI
                        case CTRL_FTP_GO://7://{"ftp_go":"on"}
                            if (val) {
                                if (au) {
                                    if (*au) {
                                        if (!strcmp(val, l_on)) { ftp_go_flag = 1; yes = 0; }
                                    }
                                }
                                done = 1;
                            }
                        break;
                        case CTRL_FTP_SRV://8://{"ftp_srv":"10.100.0.101:9221"}
                            if (val) {
                                k = strlen(val);
                                if (k > 0) {
                                    uint16_t prt = 9221;
                                    uki = strchr(val, ':');
                                    if (uki) {
                                        prt = atoi(uki + 1);
                                        *uki = '\0';
                                    }
                                    if (strlen(stz) > 16) stz[16] = 0;
                                    memset(stz, 0, sizeof(stz));
                                    strcpy(stz, val);
                                    save_param(PARAM_FTP_SRV_ADDR_NAME, (void *)stz, 16);
                                    save_param(PARAM_FTP_SRV_PORT_NAME, (void *)&prt, sizeof(uint16_t));
                                    yes = 0;
                                }
                                done = 1;
                            }
                        break;
                        case CTRL_FTP_USER://9://{"ftp_user":"login:password"}
                            if (val) {
                                char *uki = strchr(val, ':');
                                if (uki) {
                                    k = strlen(val);
                                    if ((k > 0) && (k < (34))) {
                                        memset(stz, 0, sizeof(stz));
                                        strcpy(stz, uki + 1);
                                        *uki = '\0';//val - login, stz - password
                                        if (save_param(PARAM_FTP_SRV_LOGIN_NAME, (void *)val, strlen(val)) == ESP_OK) {
                                            if (save_param(PARAM_FTP_SRV_PASSWD_NAME, (void *)stz, strlen(stz)) == ESP_OK) yes=0;
                                        }
                                    }
                                }
                                done = 1;
                            }
                        break;
#endif
                        case CTRL_GET://10://{"get":"status"}
                            if (val) {
                                if (au) {
                                    if (*au) {
                                        if (!strcmp(val, "status")) yes = 0;
                                    }
                                }
                                done = 1;
                            }
                        break;

                    }//switch (ind_c)
                }//if ((val) || (val_bin != -1))
                if (val != NULL) free(val);
                val = NULL;
                val_bin = -1;
                if (done) break;
            }//if (tmp)
        }//for
        cJSON_Delete(obj);
    }//if (obj)

    if (yes != -1) yes = ind_c;

    return yes;
}
//******************************************************************************************
time_t mk_hash(char *out, const char *part)
{
#ifdef SET_MD5
    #define hash_len 16
    const char *mark = "MD5";
#else
    #if defined(SET_SHA2_256)
        #define hash_len 32
        const char *mark = "SHA2_256";
        esp_sha_type sha_type = SHA2_256;
    #elif defined(SET_SHA2_384)
        #define hash_len 48
        const char *mark = "SHA2_384";
        esp_sha_type sha_type = SHA2_384;
    #elif defined(SET_SHA2_512)
        #define hash_len 64
        const char *mark = "SHA2_512";
        esp_sha_type sha_type = SHA2_512;
    #else
        #define hash_len 20
        const char *mark = "SHA1";
        esp_sha_type sha_type = SHA1;
    #endif
#endif
unsigned char hash[hash_len] = {0};
time_t ret = time(NULL);

    char *ts = (char *)calloc(1, (strlen(part) << 1) + 32);
    if (ts) {
        sprintf(ts,"%s_%u_%s", part, (uint32_t)ret, part);
#ifdef SET_MD5
        mbedtls_md5((unsigned char *)ts, strlen(ts), hash);
#else
        esp_sha(sha_type, (const unsigned char *)ts, strlen(ts), hash);
#endif
        free(ts);
        for (uint8_t i = 0; i < hash_len; i++) sprintf(out+strlen(out),"%02X", hash[i]);
        print_msg(1, TAGTLS, "%s hash=%s\n", mark, out);
    } else ret = 0;

    return ret;
}
//******************************************************************************************
//                    TLS server (support one client ONLY)
//******************************************************************************************
void tls_task(void *arg)
{
tls_start = 1;
total_task++;

mbedtls_entropy_context  entropy;
mbedtls_ctr_drbg_context ctr_drbg;
mbedtls_ssl_context      ssl;
mbedtls_x509_crt         srvcert;
mbedtls_pk_context       pkey;
mbedtls_ssl_config       conf;
mbedtls_net_context      server_ctx, client_ctx;

int ret = 0, len = 0, err = 0, ictrl = -1;
uint8_t auth = 0, eot = 0;
uint32_t timeout = timeout_auth;
time_t cur_time = 0;
#ifdef SET_TIMEOUT60
    time_t wait_time = 0;
#endif
char *buf = NULL, *uk = NULL, *tbuf = NULL;
char ts[64] = {0}, hash_str[256] = {0}, str_tls_port[8] = {0};
s_tls_flags flags = {
    .first = 1,
    .first_send = 0,
    .none = 0,
};
float vcc = (float)get_vcc(); vcc /= 1000;
float tChip = get_tChip();

    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);

    sprintf(str_tls_port,"%u", *(uint16_t *)arg);

    print_msg(1, TAGTLS, "TLS server task starting...(port=%s) | FreeMem=%u\n", str_tls_port, xPortGetFreeHeapSize());

    buf = (char *)calloc(1, BUF_SIZE);
    if (buf) tbuf = (char *)calloc(1, BUF_SIZE);
    if (!buf || !tbuf) goto quit1;

    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_pk_init(&pkey);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    mbedtls_ssl_config_init(&conf);

    mbedtls_entropy_init(&entropy);
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)TAGTLS, strlen(TAGTLS))) != 0) {
        ESP_LOGE(TAGTLS," failed  ! mbedtls_ctr_drbg_seed returned %d", ret);
        goto quit1;
    }

    ret = mbedtls_x509_crt_parse(&srvcert, (uint8_t *)&server_cert[0], (unsigned int)(server_cert_end - server_cert));
    if (ret < 0) {
        ESP_LOGE(TAGTLS," failed  !  mbedtls_x509_crt_parse returned -0x%x", -ret);
        goto quit1;
    }

    ret = mbedtls_pk_parse_key(&pkey, (uint8_t *)&server_key[0], (unsigned int)(server_key_end - server_key), NULL, 0);
    if (ret) {
        ESP_LOGE(TAGTLS," failed ! mbedtls_pk_parse_key returned - 0x%x", -ret);
        goto quit1;
    }

    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        ESP_LOGE(TAGTLS," failed  ! mbedtls_ssl_config_defaults returned %d", ret);
        err = ret;
        goto quit1;
    }

    mbedtls_ssl_conf_ca_chain(&conf, srvcert.next, NULL);
    if ( ( ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey) ) != 0 ) {
        ESP_LOGE(TAGTLS," failed  ! mbedtls_ssl_conf_own_cert returned %d", ret );
        goto quit1;
    }

    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);


    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        ESP_LOGE(TAGTLS," failed ! mbedtls_ssl_setup returned %d", ret);
        err = ret;
        goto quit1;
    }

    //---------         SET TIMEOUT FOR READ        -----------------------
    timeout = timeout_auth;//30000 msec
    mbedtls_ssl_conf_read_timeout(&conf, timeout);
    //---------------------------------------------------------------------

    tls_client_ip = 0;
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(struct sockaddr_in);

    while (!restart_flag) {

        memset(tls_cli_ip_addr, 0, sizeof(tls_cli_ip_addr));

        mbedtls_net_init(&server_ctx);
        mbedtls_net_init(&client_ctx);
        print_msg(1, TAGTLS, "Wait new connection...\n");

        // Bind
        ret = mbedtls_net_bind(&server_ctx, NULL, str_tls_port, MBEDTLS_NET_PROTO_TCP);
        if (ret) {
            print_msg(1, TAGTLS, " failed ! mbedtls_net_bind returned %d\n", ret);
            err = ret;
            goto exit;
        }
        ret = mbedtls_net_set_nonblock(&server_ctx);
        if (ret) print_msg(1, TAGTLS, "mbedtls_net_set_nonblock for server returned %d\n", ret);

        // Accept
        ret = MBEDTLS_ERR_SSL_WANT_READ;
        while (ret == MBEDTLS_ERR_SSL_WANT_READ) {
            ret = mbedtls_net_accept(&server_ctx, &client_ctx, NULL, 0, NULL);
            if (restart_flag) {
                eot = 1;
                err = 0;
                goto exit1;
            } else vTaskDelay(250 / portTICK_RATE_MS);//500//1000
        }
        if (ret) {
            print_msg(1, TAGTLS, " Failed to accept connection. Restarting.\n");
            mbedtls_net_free(&client_ctx);
            mbedtls_net_free(&server_ctx);
            continue;
        }
        getpeername(client_ctx.fd, (struct sockaddr *)&peer_addr, &peer_addr_len);
        strcpy(tls_cli_ip_addr, (char *)inet_ntoa(peer_addr.sin_addr));
        print_msg(1, TAGTLS, "New client %s:%u online (sock=%d)\n", tls_cli_ip_addr, htons(peer_addr.sin_port) , client_ctx.fd);
        mbedtls_ssl_set_bio(&ssl, &client_ctx, mbedtls_net_send, NULL, mbedtls_net_recv_timeout);//<- blocking I/O, f_recv == NULL, f_recv_timout != NULL

        // Handshake
        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                print_msg(1, TAGTLS, " failed ! mbedtls_ssl_handshake returned -0x%x\n", -ret);
                err = ret;
                goto exit;
            }
        }

        tls_hangup = 0;
        flags.first = 1;
        flags.first_send = 0;
#ifdef SET_TIMEOUT60
        wait_time = time(NULL);
#endif
        // Read loop
        while (!tls_hangup && !restart_flag) {
            //
            if ((!auth) && (flags.first)) {
                memset(hash_str, 0, sizeof(hash_str));
                cur_time = mk_hash(hash_str, instr);
                len = sprintf(ts,"{\"ts\":%u}\r\n", (uint32_t)cur_time);
                while ((ret = mbedtls_ssl_write(&ssl, (unsigned char *)ts, len)) <= 0) {
                    if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                        print_msg(1, TAGTLS, " failed ! mbedtls_ssl_write returned %d\n", ret);
                        err = ret;
                        break;
                    }
                }
                print_msg(1, TAGTLS, "%s", ts);
                flags.first = 0;
            }
            //
            memset(buf, 0, BUF_SIZE);
            memset(tbuf,0, BUF_SIZE);
            len = BUF_SIZE - 1;
            int rtr = mbedtls_ssl_read(&ssl, (unsigned char *)buf, len);
            if (rtr > 0) {
#ifdef SET_TIMEOUT60
                wait_time = time(NULL);
#endif
                uk = strstr(buf + 2, "\r\n"); if (uk) *uk = '\0';
                print_msg(1, TAGTLS, "Recv. data (%d bytes) from client : %s\n", rtr, buf);
                eot = 0;
                //-----------------     Check ctrl's    ------------------------------------
                ictrl = parser_json_str(buf, &auth, hash_str, &eot);
                if (auth) {
                    if (ictrl == CTRL_AUTH) {
                        print_msg(1, TAGTLS, "For client %s access granted !\n", tls_cli_ip_addr);
#ifdef UDP_SEND_BCAST
                        if (udp_start) udp_flag = 0;
#endif
                    }
                    timeout = timeout_max;
                    mbedtls_ssl_conf_read_timeout(&conf, timeout);
                }
                //------------------------------------------------------------------------
                err = 0;
            } else if (!rtr) {
                print_msg(1, TAGTLS, "Client closed connection (%d)\n", rtr);
                err = 0;
                break;
            } else {// rtr < 0  -  no data from client
                err = rtr;
                if (rtr == MBEDTLS_ERR_SSL_TIMEOUT) {// -0x6800 The operation timed out
                    if (auth) {
#ifdef SET_TIMEOUT60
                        if ( ((uint32_t)time(NULL) - (uint32_t)wait_time) >= def_idle_count ) {
                            print_msg(1, TAGTLS, "Timeout...(no data from client %u sec). Server closed connection.\n", def_idle_count);
                            break;
                        } else {
#endif
                            timeout = timeout_def;
                            mbedtls_ssl_conf_read_timeout(&conf, timeout);
#ifdef SET_TIMEOUT60
                        }
#endif
                    } else break;
                } else break;
            }

            if (!restart_flag) {
                // Write
                memset(tbuf, 0, BUF_SIZE);
                if (auth) {
                    //
                    vcc = (float)get_vcc(); vcc /= 1000;
                    tChip = get_tChip();
                    //
                    //vTaskDelay(100 / portTICK_RATE_MS);
                    len = sprintf(tbuf, "{\"DevID\":\"%08X\",\"Time\":%u,\"FreeMem\":%u,\"cli\":\"%s\",\"Vcc\":%.3f,\"Temp\":%.2f}\r\n",
                                        cli_id, (uint32_t)time(NULL), xPortGetFreeHeapSize(), tls_cli_ip_addr, vcc, tChip);
                } else len = sprintf(tbuf, "{\"status\":\"You are NOT auth. client, bye\"}\r\n");

                if (ictrl != -1) {
                    ictrl = -1;
                    while ((ret = mbedtls_ssl_write(&ssl, (unsigned char *)tbuf, len)) <= 0) {
                        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                            ESP_LOGE(TAGTLS," failed ! mbedtls_ssl_write returned %d", ret);
                            err = ret;
                            break;
                        }
                    }
                    print_msg(1, TAGTLS, "%s", tbuf);
                }
                if (!auth || eot) break;
                vTaskDelay(10 / portTICK_RATE_MS);
            }

        }//while (!tls_hangup...)


exit1:
        auth = 0;
        tls_client_ip = 0;
        mbedtls_ssl_close_notify(&ssl);

exit:
        mbedtls_ssl_session_reset(&ssl);
        mbedtls_net_free(&client_ctx);
        mbedtls_net_free(&server_ctx);

        if (err) {
            memset(hash_str, 0, sizeof(hash_str));
            mbedtls_strerror(err, hash_str, sizeof(hash_str) - 1);
            print_msg(1, TAGTLS, "Last error %d  (%s)\n", err, hash_str);
            err = 0;
        }
        if (eot) break;
#ifdef SET_TIMEOUT60
        wait_time = time(NULL);
#endif
        timeout = timeout_auth;//30000 msec
        mbedtls_ssl_conf_read_timeout(&conf, timeout);

    }

quit1:

    if (tbuf) free(tbuf);
    if (buf) free(buf);

    restart_flag = eot;

    print_msg(1, TAGTLS, "TLS server task stop | FreeMem=%u\n", xPortGetFreeHeapSize());
    if (total_task) total_task--;
    tls_start = 0;
    vTaskDelete(NULL);

}
//******************************************************************************************

#endif
