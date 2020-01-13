#ifndef __FMB630_H__
#define __FMB630_H__

#include "hdr.h"

#ifdef SET_FMB630


#define DEF_GPS_PORT 9900
#define DEF_GPS_ADDR "92.53.65.4"//"192.168.0.101"
#define buf_size 4096
#define size_imei 15
#define im_len 0x0f00
#define wait_ack_sec_def _1s * 15
#define send_period_park _1s * 60
#define send_period_move _1s * 6
#define next_try _1s * 10
#define TMR_TWO _1s * 60
#define TMR_EVT_DEF _1s * 30

#define max_pk 3
#define max_cmds 11
#define max_acks max_cmds
#define max_relay 8
#define max_pin 4
#define max_cname 9
#define max_tmp_len 1024
#define max_url_len 256
#define max_scena 10
#define len1k 1024
#define len2k 2048
#define max_log_name 128
#define DLN 384

#define max_fuel_tank 750	// 42 liters
#define min_fuel_tank 3300	// < 2 liters

//-----------------------------------------------------------------------

/*
//направление движения:приращение координаты:количество замеров по сценарию
West:1000:10    ; mirror.gsm.longitude-0.001000, mirror.gsm.angle=270 | Запад угол = 270^
North:1000:10   ; mirror.gsm.latitude+0.001000 , mirror.gsm.angle=0   | Север угол = 0^
East:1000:10    ; mirror.gsm.longitude+0.001000, mirror.gsm.angle=90  | Восток угол = 90^
South:1000:10   ; mirror.gsm.latitude-0.001000 , mirror.gsm.angle=180 | Юг угол = 180^
;остановка:длительность остановки в замерах (каждые 20 сек - 1 замер)
Stop:30
*/

typedef enum {
    PARK = 0,
    NORTH,
    EAST,
    SOUTH,
    WEST,
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST,
    STOP
} rumb_t;

#pragma pack(push,1)
typedef struct
{
    rumb_t dir;//направление или стоп
    int delta;//приращение координаты
    int kols;//количество замеров
} s_ones;
typedef struct s_rec {
    int index;
    struct s_rec *next;
    s_ones ones;
} s_rec;
typedef struct {
    struct s_rec *first;
    uint32_t counter;
} s_hdr_rec;
#pragma pack(pop)


//-----------------------------------------------------------------------

#pragma pack(push,1)
typedef struct
{
    char srv[max_url_len];//"server=",//127.0.0.1
    uint16_t port;//9090
    char imei[size_imei];//"imei=",//351580051430037
    uint8_t mode;//"mode="//0
    uint32_t snd_park;//"period_park="//30
    uint32_t snd_move;//"period_move="//10
    uint32_t wait_ack;//"wait_ack="//15
    uint32_t wait_before_new_connect;//"wait_before_new_connect=",//3
    //location=54.699680,20.514002
    uint32_t latitude;// широта
    uint32_t longitude;// долгота
} s_conf;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    uint8_t stat;
    uint32_t time;
} s_box;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct
{
    uint16_t len;
    char imei[size_imei];
} s_imei;
typedef struct
{
    uint32_t zero;
    uint32_t len;
} s_avl_hdr;
#pragma pack(pop)


#pragma pack(push,1)
typedef struct
{
    uint32_t zero;      //0x00000000
    uint32_t tlen;      //data len without crc
    uint8_t codec_id; //0x0c
    uint8_t cmd_cnt;  //command quantity
    uint8_t cmd_type; //command type
    uint32_t blen;      //body len without crc
} s_avl_cmd;
#pragma pack(pop)


typedef struct {
    uint8_t codec_id;
    uint8_t numbers_pack;
    uint8_t type;
    uint32_t len;
} s_hdr_ack;

#pragma pack(push,1)
typedef struct {
    uint8_t codec_id;
    uint8_t numbers_pack;
} s_hdr_pack;
typedef struct {
    uint64_t ts;
    uint8_t prio;
    uint32_t longitude;
    uint32_t latitude;
    short altitude;
    uint16_t angle;
    uint8_t sattelites;
    uint16_t speed;
    uint8_t id_event;
    uint8_t total_elem;
} s_gsm;
typedef struct {
    uint8_t total_len1;
    uint8_t id_din1;
    uint8_t val_din1;//DIN1 - Ign : 1-вкл.  0-выкл.
    uint8_t id_din2;
    uint8_t val_din2;//DIN2 - Door : 0-отрк.  1-закр.
    uint8_t id_din3;
    uint8_t val_din3;//DIN3 - Trunk : 0-откр.  1-закр.
    uint8_t id_din4;
    uint8_t val_din4;//DIN4 - Skp : 1-парковка
    uint8_t id_dout1;
    uint8_t val_dout1;//DOUT1 - бензонасос : 1-разблокирован  0-заблокирован
    uint8_t id_dout2;
    uint8_t val_dout2;
    uint8_t id_dout3;
    uint8_t val_dout3;
    uint8_t id_dout4;
    uint8_t val_dout4;
    uint8_t id_pfile;
    uint8_t val_pfile;
    uint8_t id_GNSS;
    uint8_t val_GNSS;
    uint8_t id_moves;
    uint8_t val_moves;
    uint8_t id_gsml;
    uint8_t val_gsml;
    uint8_t id_ignition;
    uint8_t val_ignition;//aka DIN1
} s_io1;
typedef struct {
    uint8_t total_len2;
    uint8_t id_ain1;
    uint16_t val_ain1;//AIN1 - fuel (ДУТ)
    uint8_t id_ain2;
    uint16_t val_ain2;//AIN2 - tach (Тахометр) : if (val > 1000) tah = 1;//двигатель заведен  else tah = 0;//двигатель заглушен
    uint8_t id_ain3;
    uint16_t val_ain3;//AIN3 - hood (Капот) : if (val < 1000) hood = 1;//открыт else hood = 0;//закрыт
    uint8_t id_bvolt;
    uint16_t val_bvolt;
    uint8_t id_bcur;
    uint16_t val_bcur;
    uint8_t id_evolt;
    uint16_t val_evolt;
    uint8_t id_speed;
    uint16_t val_speed;
} s_io2;
typedef struct {
    uint8_t total_len4;
    uint8_t id_odo;
    uint32_t val_odo;
    uint8_t id_oper;
    uint32_t val_oper;
    uint8_t id_todo;
    uint32_t val_todo;
} s_io4;
typedef struct {
    uint8_t total_len8;
} s_io8;
typedef struct {
    uint8_t numbers_pack;
    uint16_t zero;
} s_tail_pack;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct {
    s_gsm gsm;
    s_io1 io1;
    s_io2 io2;
    s_io4 io4;
    s_io8 io8;
} s_mir;
#pragma pack(pop)

extern const char *TAGGPS;


//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************

/*
extern const char *scena[];
extern s_hdr_rec hdr_rec;

extern int QuitAll;

extern char time_str[TIME_STR_LEN];

extern const char *im_def;
extern char im[size_imei + 1];
extern s_imei avl_imei;

extern s_mir mirror;

extern s_box box[max_relay];
extern uint32_t tmr_box[max_relay];

extern s_box pin[max_pin];
extern uint32_t tmr_pin[max_pin];

extern uint32_t tmr_send;
extern uint32_t tmr_ack;
extern uint32_t tmr_two;
extern uint32_t tmr_evt;
extern uint32_t TMR_EVENT;

extern uint8_t Prio;

extern int fd_log;
extern int max_size_log;
extern const char *fm_log_tail;
extern char fm_log[max_log_name];
extern const char *cfg_file_def;

extern uint8_t work_mode;

extern int scena_kols;
extern int scena_delta;
extern int scena_index;
extern int scena_total;

extern uint32_t wait_ack_sec;

extern uint32_t sp_park;
extern uint32_t sp_move;
extern uint32_t send_period;
extern uint32_t wait_before_new_connect;

extern uint32_t last_send;

extern uint8_t move_flag;
extern uint8_t stop_flag;

extern s_conf conf;
*/

//--------------------------------------------------------------------------------------------------

extern int read_cfg(s_conf *cf, const char *fn, uint8_t prn);
extern void fmb630_task(void *arg);

/*
extern void init_mirror(uint32_t *lat, uint32_t *lon);
extern int make_record(uint8_t *buf, uint8_t kolpack, uint8_t wmode);
extern int parse_cmd(uint8_t ct,  // command_type
                    char *com,          // command string
                    uint8_t *out, //output buffer
                    int *ci);           // index of command
extern int read_cfg(const char *fn, uint8_t prn);

extern int add_ones(s_ones *data, uint8_t flag);
extern s_rec *get_ones(int ind, s_ones *data, uint8_t flag);
extern int find_ind(rumb_t rumb, uint8_t flag);
extern void del_all_ones(uint8_t flag);
extern void prn_all_ones(uint8_t flag);
*/

#endif

#endif
