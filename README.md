#############################################################################################
##       esp_fmb630 - software emulator Teltonika FMB630 device on ESP32 platform
#############################################################################################


## Состав проекта:

* Hardware part : esp32(DevKitC board) + ssd1306(i2c) + SDCard + 2LED

* Software part : fmb630_client + ftp_client + sntp_server + udp_server + log_server + FreeRTOS


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


## Пример логов при работе модуля :
```
App version 3.9 (19.01.2020) | MAC 24:0a:c4:05:ef:90 | SDK Version v4.1-dev-1795-gca8fac8 | FreeMem 278000
[VFS] Started timer with period 100 ms, time since boot: 2531025/0
[VFS] DEVICE_ID='C405EF90'
[VFS] SNTP_SERVER '2.ru.pool.ntp.org' TIME_ZONE 'EET-2'
[VFS] WIFI_MODE (1): STA
[VFS] WIFI_STA_PARAM: 'ssid:password'
[VFS] FTP SERVER : '192.168.0.101:9221' (login/passwd)
[WIFI] WIFI_MODE - STA: 'ssid':'password'
[WIFI] Connected to AP 'ssid' auth(3):'AUTH_WPA2_PSK' chan:11 rssi:-36
[WIFI] Local ip_addr : 192.168.0.100
[NTP] Start sntp_task | FreeMem 240768
[NTP] Getting time from SNTP server '2.ru.pool.ntp.org'.
[LOG] Start NetLogServer task (port=8008)| FreeMem 235836
[LOG] Wait new log_client... | FreeMem 235468
[NTP] The current date/time is: Sun Jan 19 22:10:41 2020 EET-2
[NTP] Stop sntp_task | FreeMem 235360
19.01 22:10:42 [CARD] Failed to initialize the sdcard
19.01 22:10:42 [DISK] Mount FATFS partition 'disk' on device '/spiflash' OK (size=966656)
19.01 22:10:42 [DISK] === GPIO_DELIT_PIN(2)=1 ===
19.01 22:10:42 [DISK] Open DIR '/spiflash/':
        file: type=1 name='CONF.TXT' size=393
        file: type=1 name='CONFIG.TXT' size=393
19.01 22:10:42 [GPS] Configuration file '/spiflash/conf.txt' present:
[GPS] Start fmb630 task | FreeMem 200204
19.01 22:10:42 [GPS] START FMB630 :
        srv=192.168.0.101:9900
        imei=351580051430040
        mode=1
        send_period=30/2
        wait_ack=15
        wait_before_new_connect=15
        location=55.803890,37.403015
        total_scena=8
19.01 22:10:42 [GPS] Total records in list 8 :
        [1] 'Park'(0):0:5
        [2] 'North-West'(6):1500:20
        [3] 'North-East'(5):1500:20
        [4] 'South-East'(7):1500:20
        [5] 'South-West'(8):1500:20
        [6] 'South'(3):1500:20
        [7] 'North'(1):1500:20
        [8] 'Stop'(9):0:5
19.01 22:10:43 [GPS] Connect to 192.168.0.101:9900 OK
19.01 22:10:43 [GPS] data to server (17):00 0F 33 35 31 35 38 30 30 35 31 34 33 30 30 34 30
19.01 22:10:43 [GPS] data from server (1):01
19.01 22:10:43 [GPS] Access ganted !!!
19.01 22:10:43 [GPS] Get record [1] 'Park'(0):0:5
19.01 22:10:43 [GPS] [1] msg #1 from 5
19.01 22:10:43 [GPS] Send packet #1 with len=107 to server:
000000000000005F08010000016FBF6DD1B800164B3F402142FF800008000003000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000100003FAA
19.01 22:10:43 [GPS] data from server (4) :00000001
19.01 22:10:43 [FTP] Start ftp_client task | FreeMem 172208
19.01 22:10:44 [FTP] Stop ftp_client task (ret=1) | FreeMem 171420
19.01 22:10:44 [DISK] Unmounted FAT filesystem partition 'disk' | FreeMem 208428
19.01 22:11:08 [LOG] New log_client 192.168.0.101:38428 (soc=56) online | FreeMem 208044
19.01 22:11:13 [GPS] Get record [1] 'Park'(0):0:5
19.01 22:11:13 [GPS] [1] msg #1 from 5
19.01 22:11:13 [GPS] Send packet #2 with len=107 to server:
000000000000005F08010000016FBF6E46E800164B3F402142FF80000800000B000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000100008DEB
19.01 22:11:13 [GPS] data from server (4) :00000001
19.01 22:11:43 [GPS] Get record [1] 'Park'(0):0:5
19.01 22:11:43 [GPS] [1] msg #1 from 5
19.01 22:11:43 [GPS] Send packet #3 with len=199 to server:
00000000000000BB08020000016FBF6EBC1800164B3F402142FF800008000009000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000000016FBF6EBC1800164B3F402142FF800008000009000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000200004695
19.01 22:11:43 [GPS] data from server (4) :00000002
19.01 22:11:46 [GPS] Error code 0x02 :
        Server closed connection without answer.
19.01 22:11:46 [GPS] Sleep 15 sec before try next connection...(ses 63 sec.) | FreeMem 208028
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

