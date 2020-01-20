#ifndef __FTP_CLI_H_
#define __FTP_CLI_H_

#include "hdr.h"

#ifdef SET_FTP_CLI


#define FTP_SRV_PORT_DEF 9221
#define FTP_SRV_ADDR_DEF "192.168.0.101"
#define FTP_SRV_LOGIN_DEF "test"
#define FTP_SRV_PASSWD_DEF "9999"
#define FTP_PATH_DEF "/spiflash"
#define FTP_CONF_DEF "conf.txt"


#define FTP_CLIENT_DEBUG

#define FTP_CLIENT_BUFFER_SIZE          2048 //4096
#define FTP_CLIENT_RESPONSE_BUFFER_SIZE 1024
#define FTP_CLIENT_TEMP_BUFFER_SIZE     1024
#define FTP_CLIENT_ACCEPT_TIMEOUT         10

/* FtpAccess() type codes */
#define FTP_CLIENT_DIR         1
#define FTP_CLIENT_DIR_VERBOSE 2
#define FTP_CLIENT_FILE_READ   3
#define FTP_CLIENT_FILE_WRITE  4
#define FTP_CLIENT_MLSD        5

/* FtpAccess() mode codes */
#define FTP_CLIENT_ASCII  'A'
#define FTP_CLIENT_IMAGE  'I'
#define FTP_CLIENT_TEXT   FTP_CLIENT_ASCII
#define FTP_CLIENT_BINARY FTP_CLIENT_IMAGE

/* connection modes */
#define FTP_CLIENT_PASSIVE 1
#define FTP_CLIENT_ACTIVE  2

/* connection option names */
#define FTP_CLIENT_CONNMODE      1
#define FTP_CLIENT_CALLBACK      2
#define FTP_CLIENT_IDLETIME      3
#define FTP_CLIENT_CALLBACKARG   4
#define FTP_CLIENT_CALLBACKBYTES 5


//#if !defined FTP_CLIENT_DEFAULT_MODE
#define FTP_CLIENT_DEFAULT_MODE FTP_CLIENT_PASSIVE
//#endif
#define FTP_CLIENT_CONTROL 0
#define FTP_CLIENT_READ    1
#define FTP_CLIENT_WRITE   2

typedef struct NetBuf NetBuf_t;

typedef int (*FtpClientCallback_t)(NetBuf_t *nControl, uint32_t xfered, void *arg);

typedef struct
{
    FtpClientCallback_t cbFunc; /* function to call */
    void *cbArg; /* argument to pass to function */
    unsigned int bytesXferred; /* callback if this number of bytes transferred */
    unsigned int idleTime; /* callback if this many milliseconds have elapsed */
} FtpClientCallbackOptions_t;

typedef struct
{
    /*Miscellaneous Functions*/
    int (*ftpClientSite)(const char *cmd, NetBuf_t *nControl);
    char* (*ftpClientGetLastResponse)(NetBuf_t *nControl);
    int (*ftpClientGetSysType)(char *buf, int max, NetBuf_t *nControl);
    int (*ftpClientGetFileSize)(const char *path, unsigned int *size, char mode, NetBuf_t *nControl);
    int (*ftpClientGetModDate)(const char *path, char *dt, int max, NetBuf_t *nControl);
    int (*ftpClientSetCallback)(const FtpClientCallbackOptions_t *opt, NetBuf_t *nControl);
    int (*ftpClientClearCallback)(NetBuf_t *nControl);
    /*Server connection*/
    int (*ftpClientConnect)(const char *host, uint16_t port, NetBuf_t **nControl);
    int (*ftpClientLogin)(const char *user, const char *pass, NetBuf_t *nControl);
    void (*ftpClientQuit)(NetBuf_t *nControl);
    int (*ftpClientSetOptions)(int opt, long val, NetBuf_t *nControl);
    /*Directory Functions*/
    int (*ftpClientChangeDir)(const char *path, NetBuf_t *nControl);
    int (*ftpClientMakeDir)(const char *path, NetBuf_t *nControl);
    int (*ftpClientRemoveDir)(const char *path, NetBuf_t *nControl);
    int (*ftpClientDir)(const char *outputfile, const char *path, NetBuf_t *nControl);
    int (*ftpClientNlst)(const char *outputfile, const char *path, NetBuf_t *nControl);
    int (*ftpClientMlsd)(const char *outputfile, const char *path, NetBuf_t *nControl);
    int (*ftpClientChangeDirUp)(NetBuf_t *nControl);
    int (*ftpClientPwd)(char *path, int max, NetBuf_t *nControl);
    /*File to File Transfer*/
    int (*ftpClientGet)(const char *outputfile, const char *path, char mode, NetBuf_t *nControl);
    int (*ftpClientPut)(const char *inputfile, const char *path, char mode, NetBuf_t *nControl);
    int (*ftpClientDelete)(const char *fnm, NetBuf_t *nControl);
    int (*ftpClientRename)(const char *src, const char *dst, NetBuf_t *nControl);
    /*File to Program Transfer*/
    int (*ftpClientAccess)(const char *path, int typ, int mode, NetBuf_t *nControl, NetBuf_t **nData);
    int (*ftpClientRead)(void *buf, int max, NetBuf_t *nData);
    int (*ftpClientWrite)(const void *buf, int len, NetBuf_t *nData);
    int (*ftpClientClose)(NetBuf_t *nData);
} FtpClient;


typedef struct {
    char devSrv[ftp_pole_len];
    esp_err_t devMnt;
    uint16_t devPort;
    char devPath[ftp_pole_len];
    char devConf[ftp_pole_len];
    char devLogin[ftp_pole_len];
    char devPasswd[ftp_pole_len];
} s_ftp_var;


extern const char *TAGFTP;
extern uint8_t ftp_start;
extern int getFileOK;

extern FtpClient *getFtpClient(void);
extern void ftp_cli_task(void *arg);


#endif /* SET_FTP_CLI */

#endif /* __FTP_CLI_H_ */
