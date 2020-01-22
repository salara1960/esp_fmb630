//#define UDP_SEND_BCAST

#define SET_FTP_CLI
#define SET_TLS_SRV
#define SET_FATDISK
#define SET_FMB630
#define SET_SDCARD
#define SET_NET_LOG
#define SET_SSD1306
#define SET_SNTP

#ifdef SET_TLS_SRV
    #undef SET_TIMEOUT60
#endif
