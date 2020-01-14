#undef UDP_SEND_BCAST

//#define SET_TLS_SRV
#define SET_FATDISK
#define SET_FMB630
#define SET_SDCARD
#define SET_NET_LOG
#define SET_SSD1306
#define SET_SNTP
#define SET_ERROR_PRINT

#ifdef SET_TLS_SRV
    #undef SET_TIMEOUT60
#endif
