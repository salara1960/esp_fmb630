#############################################################################################
##       esp_fmb630 - software emulator Teltonika FMB630 device on ESP32 platform
#############################################################################################


## Состав проекта:

* Hardware part : esp32(DevKitC board) + ssd1306(i2c) + SDCard + 2LED + CATALEX Touch sensor(restart pin)

* Software part : fmb630_client + ftp_client + sntp_client + udp_client + log_server + tls_server + FreeRTOS


## Файлы пакета:

* sdkconfing     - файл конфигурации проекта

* Makefile       - make файл (файл сценария компиляции проекта)

* version.c      - файл версии ПО

* README.md      - файл справки

* main/          - папка исходников

* partitions.csv - файл конфигурации разделов dataflah

* conf.txt       - файл конфигурации режима/сценария работы устройства (размещается на sdcard)


## Требуемые компоненты:

```
- Cross compiler xtensa-esp32-elf (http://esp-idf-fork.readthedocs.io/en/stable/linux-setup.html#step-0-prerequisites)
- SDK esp-idf (https://github.com/espressif/esp-idf)
- Python2 (https://www.python.org/)
```

## Компиляция и запись проекта

make menuconfig - конфигурация проекта

make app        - компиляция проекта

make flash      - запись бинарного кода проекта в dataflash


## Поддерживаемые команды:

```
- "getgps",        //{"command":0}
- "setdigout",     //{"command":1, "relay":1, "time":0} // 1XXX 0 0 0 0"
- "setdigout",     //{"command":2, "relay":1, "time":0} // 0XXX 0 0 0 0"
- "getio",         //{"command":15}
- "getver",        //{"command":20}
- "cpureset",      //{"command":26}
- "deleterecords", //{"command":29}
- "SET_ON",        //{"command":33, "relay":X, "time":Y}
- "SET_OFF"        //{"command":34, "relay":X}
- "GET_STAT",      //{"command":35}
- "#DO REPORT",    //{"command":40}
- "SET_ALL",       //{"command":48,"param":"X1,Y1 X2,Y2 X3,Y3 X4,Y4 X5,Y5 X6,Y6 X7,Y7 X8,Y8"}
```

## Команды контроля параметров для TLS сервера :

```
- {"auth":"hash"} - hash md5_from_key_word
- {"udp":"on"} , {"udp":"off"}
- {"sntp":"on"}
- {"sntp_srv":"ip_ntp_server"}
- {"time_zone":"UTC+02:00"}
- {"restart":"on"}
- {"time":"1493714647"} , {"time":1493714647}
- {"ftp_go":"on"}
- {"ftp_srv":"10.100.0.201:21"}
- {"ftp_user":"user:password"}
- {"get":"status"},{"get":"wifi"},{"get":"sntp_srv"},{"get":"time_zone"},{"get":"ftp_srv"},{"get":"ftp_user"},{"get":"log_port"},{"get":"version"},{"get":"gps_srv"}
```


## Пример логов при работе модуля :
```
App version 4.2 (22.01.2020) | MAC 24:0a:c4:05:ef:90 | SDK Version v4.1-dev-1935-g647cb62 | FreeMem 275696
[VFS] Started timer with period 100 ms, time since boot: 2530048/0
[VFS] DEVICE_ID='C405EF90'
[VFS] SNTP_SERVER '2.ru.pool.ntp.org' TIME_ZONE 'EET-2'
[VFS] WIFI_MODE (1): STA
[VFS] WIFI_STA_PARAM: 'ssid:password'
[VFS] TLS_PORT: 4545
[VFS] FTP SERVER : '192.168.0.201:21' (login/passwd)
[WIFI] WIFI_MODE - STA: 'ssid':'password'
[WIFI] Connected to AP 'ssid' auth(3):'AUTH_WPA2_PSK' chan:3 rssi:-36
[NTP] Start sntp_task | FreeMem 238252
[LOG] Start NetLogServer task (port=8008)| FreeMem 233680
[WIFI] Local ip_addr : 192.168.0.100
[NTP] Getting time from SNTP server '2.ru.pool.ntp.org'.
[LOG] Wait new log_client... | FreeMem 232772
[NTP] The current date/time is: Wed Jan 22 12:16:47 2020 EET-2
[NTP] Stop sntp_task | FreeMem 232840
22.01 12:16:48 [CARD] Failed to initialize the sdcard
22.01 12:16:49 [DISK] Mount FATFS partition 'disk' on device '/spiflash' OK (size=966656)
22.01 12:16:49 [DISK] === GPIO_DELIT_PIN(2)=1 ===
22.01 12:16:49 [DISK] Open DIR '/spiflash/':
        file: type=1 name='CONF.TXT' size=393
22.01 12:16:50 [GPS] Configuration file '/spiflash/conf.txt' present:
[GPS] Start fmb630 task | FreeMem 197692
22.01 12:16:50 [GPS] START FMB630 :
        srv=192.168.0.101:9900
        imei=351580051430040
        mode=1
        send_period=30/2
        wait_ack=15
        wait_before_new_connect=15
        location=55.803890,37.403015
        total_scena=8
22.01 12:16:50 [GPS] Total records in list 8 :
        [1] 'Park'(0):0:5
        [2] 'North-West'(6):1500:20
        [3] 'North-East'(5):1500:20
        [4] 'South-East'(7):1500:20
        [5] 'South-West'(8):1500:20
        [6] 'South'(3):1500:20
        [7] 'North'(1):1500:20
        [8] 'Stop'(9):0:5
22.01 12:16:50 [GPS] Connect to 192.168.0.101:9900 OK
22.01 12:16:50 [GPS] data to server (17):00 0F 33 35 31 35 38 30 30 35 31 34 33 30 30 34 30
22.01 12:16:50 [GPS] data from server (1):01
22.01 12:16:50 [GPS] Access ganted !!!
22.01 12:16:50 [GPS] Get record [1] 'Park'(0):0:5
22.01 12:16:50 [GPS] [1] msg #1 from 5
22.01 12:16:50 [GPS] Send packet #1 with len=107 to server:
000000000000005F08010000016FCCC12E5000164B3F402142FF800008000003000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD80000000000010000F1B4
22.01 12:16:50 [GPS] data from server (4) :00000001
22.01 12:16:51 [FTP] Start ftp_client task | FreeMem 169604
22.01 12:16:51 [FTP] [connectFtpClient] Try connect to FTP server 192.168.0.201:21
22.01 12:16:51 [FTP] [connectFtpClient] FTP Client Error: connect() error
22.01 12:16:51 [FTP] Stop ftp_client task (ret=0) | FreeMem 169360
22.01 12:16:52 [TLS] TLS server task starting...(port=4545) | FreeMem=169432
22.01 12:16:52 [TLS] Wait new connection...
22.01 12:16:53 [DISK] Unmounted FAT filesystem partition 'disk' | FreeMem 171204
22.01 12:17:20 [GPS] Get record [1] 'Park'(0):0:5
22.01 12:17:20 [GPS] [1] msg #1 from 5
22.01 12:17:20 [GPS] Send packet #2 with len=107 to server:
000000000000005F08010000016FCCC1A38000164B3F402142FF80000800000B000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000100003AE6
22.01 12:17:20 [GPS] data from server (4) :00000001
22.01 12:17:50 [GPS] Get record [1] 'Park'(0):0:5
22.01 12:17:50 [GPS] [1] msg #1 from 5
22.01 12:17:50 [GPS] Send packet #3 with len=199 to server:
00000000000000BB08020000016FCCC218B000164B3F402142FF800008000009000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000000016FCCC218B000164B3F402142FF800008000009000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD80000000000020000A337
22.01 12:17:50 [GPS] data from server (4) :00000002
22.01 12:18:13 [LOG] New log_client 192.168.0.101:50654 (soc=57) online | FreeMem 170644
22.01 12:18:20 [GPS] Get record [1] 'Park'(0):0:5
22.01 12:18:20 [GPS] [1] msg #1 from 5
22.01 12:18:20 [GPS] Send packet #4 with len=199 to server:
00000000000000BB08020000016FCCC28DE000164B3F402142FF800008000003000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000000016FCCC28DE000164B3F402142FF800008000003000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000200007AE4
22.01 12:18:20 [GPS] data from server (4) :00000002
.
.
.
```


## P.S.
```
* 'nc ip_addr_device 8008' - getting logs in realtime from linux command line

* configuration file example:
server=localhost:9900                    | car-server's url
imei=351580051430040                     | imei device
mode=0                                   | work mode (0-parking, 1-trip)
period_park=30                           | in parking mode send period (sec)
period_move=5                            | in moveing mode send period (sec)
wait_ack=10                              | wait ack from car-server (sec)
wait_before_new_connect=5                | wait before try new connection to server
location=54.699680,20.514002             | first location of device
;
;направление движения:приращение координаты:количество замеров по сценарию
;
;остановка:длительность остановки в замерах (каждые 30 сек - 1 замер)
#Park:5
; latitude+0.001000, longitude-0.001000, angle=315      | Северо-Запад - 315^
#North-West:1000:50
; latitude+0.001000, longitude+0.001000, angle=45       | Северо-Восток угол = 45^
#North-East:1000:50
; latitude-0.001000, longitude+0.001000, angle=135      | Юго-Восток угол = 135^
#South-East:1000:50
; latitude-0.001000, longitude-0.001000, angle=225      | Юго-Запад угол = 225^
#South-West:1000:50
; longitude-0.001000, angle=270                         | Запад угол = 270^
#West:-1000:50
; latitude-0.001000 , angle=180                         | Юг угол = 180^
#South:-1000:50
; longitude+0.001000, angle=90                          | Восток угол = 90^
#East:1000:50
; latitude+0.001000 , angle=0                           | Север угол = 0^
#North:1000:50
;остановка:длительность остановки в замерах (каждые 30 сек - 1 замер)
#Stop:10
```

