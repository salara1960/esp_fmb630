#############################################################################################
#
#       esp_fmb630 - software emulator Teltonika FMB630 device on ESP32 platform
#
#############################################################################################


## Состав проекта:

* Hardware part : esp32(DevKitC board) + ssd1306(i2c) + SDCard

* Software part : fmb630_client + sntp_server + udp_server + log_server + FreeRTOS


## Файлы пакета:

* sdkconfing     - файл конфигурации проекта

* Makefile       - make файл (файл сценария компиляции проекта)

* version.c      - файл версии ПО

* README.md      - файл справки

* main/          - папка исходников

* partitions.csv - файл конфигурации разделов dataflah

* conf.txt       - файл конфигурации режима/сценария работы устройства (размещается на sdcard)


##Требуемые компоненты:

```
- Cross compiler xtensa-esp32-elf (http://esp-idf-fork.readthedocs.io/en/stable/linux-setup.html#step-0-prerequisites)
- SDK esp-idf (https://github.com/espressif/esp-idf)
- Python2 (https://www.python.org/)
```

## Компиляция и запись проекта

make menuconfig - конфигурация проекта

make app        - компиляция проекта

make flash      - запись бинарного кода проекта в dataflash


## Пример логов при работе модуля :
```
App version 3.2 (13.01.2020) | SDK Version v4.1-dev-1795-gca8fac8 | FreeMem 279464
[VFS] Started timer with period 100 ms, time since boot: 2527919/0
[VFS] DEVICE_ID='C405EF90'
[VFS] SNTP_SERVER '2.ru.pool.ntp.org' TIME_ZONE 'EET-2'
[VFS] WIFI_MODE (1): STA
[VFS] WIFI_STA_PARAM: 'ssid:password'
[VFS] TLS_PORT: 4545
[WIFI] WIFI_MODE - STA: 'ssid':'password'
[WIFI] Connected to AP 'ssid' auth(3):'AUTH_WPA2_PSK' chan:2 rssi:-26
[WIFI] Local ip_addr : 192.168.0.100
[LOG] Start NetLogServer task (port=8008)| FreeMem 238596
[LOG] Wait new log_client... | FreeMem 238232
[NTP] Start sntp_task | FreeMem 235820
[NTP] Getting time from SNTP server '2.ru.pool.ntp.org'.
[NTP] The current date/time is: Mon Jan 13 12:53:07 2020 EET-2
[NTP] Stop sntp_task | FreeMem 235360
[FAT] Mount /sdcard OK | FreeMem 208848
Name: SD01G
Type: SDSC
Speed: 20 MHz
Size: luMB
13.01 12:53:08 [FAT] Open DIR '/sdcard/':
        file: type=1 name='CONF.TXT' size=656
[TLS] TLS server task starting...(port=4545) | FreeMem=199524
13.01 12:53:08 [TLS] Wait new connection...
13.01 12:53:09 [GPS] Configuration file '/sdcard/conf.txt' present:
;--------------------------------------------------------
server=192.168.0.101:9900
imei=351580051430040
mode=1
period_park=30
period_move=5
wait_ack=15
wait_before_new_connect=10
location=54.699680,20.514002
;направление движения:приращение координаты:количество замеров по сценарию
;остановка:длительность остановки в замерах (каждые 30 сек - 1 замер)
#Park:5
#North-West:1000:15
#North-East:1000:15
#South-East:1000:15
#South-West:1000:15
#South:1000:15
#North:1000:15
#Stop:5
;--------------------------------------------------------
13.01 12:53:09 [FAT] SD Card /sdcard unmounted | FreeMem 202000
[GPS] Start fmb630 task | FreeMem 191400
13.01 12:53:09 [GPS] START FMB630 :
        srv=192.168.0.101:9900
        imei=351580051430040
        mode=1
        send_period=30/5
        wait_ack=15
        wait_before_new_connect=10
        location=54.699680,20.514002
        total_scena=8
13.01 12:53:09 [GPS] Total records in list 8 :
        [1] 'Park'(0):0:5
        [2] 'North-West'(6):1000:15
        [3] 'North-East'(5):1000:15
        [4] 'South-East'(7):1000:15
        [5] 'South-West'(8):1000:15
        [6] 'South'(3):1000:15
        [7] 'North'(1):1000:15
        [8] 'Stop'(9):0:5
13.01 12:53:09 [GPS] Connect to 192.168.0.101:9900 OK
13.01 12:53:09 [GPS] data to server (17):00 0F 33 35 31 35 38 30 30 35 31 34 33 30 30 34 30
13.01 12:53:10 [GPS] data from server (1):01
13.01 12:53:10 [GPS] Access ganted !!!
13.01 12:53:10 [GPS] Get record [1] 'Park'(0):0:5
13.01 12:53:10 [GPS] [1] msg #1 from 5
13.01 12:53:10 [GPS] Send packet #1 with len=107 to server:
000000000000005F08010000016F9E8935F0000C3A3030209A82400008000003000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD800000000000100005A6B
13.01 12:53:10 [GPS] data from server (4) :00000001
13.01 12:53:40 [GPS] Get record [1] 'Park'(0):0:5
13.01 12:53:40 [GPS] [1] msg #1 from 5
13.01 12:53:40 [GPS] Send packet #2 with len=107 to server:
000000000000005F08010000016F9E89AB20000C3A3030209A8240000800000B000000170D0100020103010401B300B4003200330016034703F0001504EF00070902EE0A01460B04B04326C5440032422F0C18000003C700000000F10000620BD80000000000010000F176
13.01 12:53:40 [GPS] data from server (4) :00000001
13.01 12:53:44 [GPS] data from server (44) :00000000000000200C0105000000187365746469676F7574203158585820302030203020300D0A0100000B85
        cmd='setdigout 1XXX 0 0 0 0'
13.01 12:53:44 [GPS] ret=60 len_ks=48 ack:DOUTS are set to: 1000 TMOs are: 0 0 0 0
13.01 12:53:44 [GPS] Send ack for cmd(6)='setdigout 1XXX 0 0 0 0' :
00000000000000300C010600000028444F555453206172652073657420746F3A203130303020544D4F73206172653A20302030203020300100001EC7
13.01 12:54:10 [GPS] Get record [1] 'Park'(0):0:5
13.01 12:54:10 [GPS] Car move now....
13.01 12:54:10 [GPS] [1] msg #255 from 5
13.01 12:54:10 [GPS] Send packet #3 with len=199 to server:
00000000000000BB08020000016F9E8A2050000C3A3030209A82400008000009000000170D0101020103010400B301B4003200330016034703F0001504EF01070902EE0A06000B04B04326C544003242329618000003C700000000F10000620BD800000000000000016F9E8A2050000C3A3030209A82400008000009000000170D0101020103010400B301B4003200330016034703F0001504EF01070902EE0A06000B04B04326C544003242329618000003C700000000F10000620BD800000000000200003DDE
13.01 12:54:10 [GPS] data from server (4) :00000002
13.01 12:54:15 [GPS] Get record [2] 'North-West'(6):1000:15
13.01 12:54:15 [GPS] [2] msg #1 from 15
13.01 12:54:15 [GPS] Send packet #4 with len=199 to server:
00000000000000BB08020000016F9E8A33D8000C3A2C48209A86280007013B09000900170D0101020103010400B301B4003200330016034703F0011504EF01070902EE0A05EE0B04B04326C544003242329618000903C70000000AF10000620BD80000000A000000016F9E8A33D8000C3A2C48209A86280007013B09000900170D0101020103010400B301B4003200330016034703F0011504EF01070902EE0A05EE0B04B04326C544003242329618000903C70000000AF10000620BD80000000A00020000BDEA
13.01 12:54:15 [GPS] data from server (4) :00000002
13.01 12:54:20 [GPS] Get record [2] 'North-West'(6):1000:15
13.01 12:54:20 [GPS] [2] msg #2 from 15
13.01 12:54:20 [GPS] Send packet #5 with len=199 to server:
00000000000000BB08020000016F9E8A4760000C3A2860209A8A100006013B09000A00170D0101020103010400B301B4003200330016034703F0011504EF01070902EE0A05DC0B04B04326C544003242329618000A03C70000000AF10000620BD800000014000000016F9E8A4760000C3A2860209A8A100006013B09000A00170D0101020103010400B301B4003200330016034703F0011504EF01070902EE0A05DC0B04B04326C544003242329618000A03C70000000AF10000620BD80000001400020000D0F5
13.01 12:54:20 [GPS] data from server (4) :00000002
13.01 12:54:24 [GPS] Get record [2] 'North-West'(6):1000:15
13.01 12:54:24 [GPS] [2] msg #3 from 15
13.01 12:54:24 [GPS] Send packet #6 with len=199 to server:
00000000000000BB08020000016F9E8A5700000C3A2478209A8DF80005013B09000B00170D0101020103010400B301B4003200330016034703F0011504EF01070902EF0A05CA0B04B04326C544003242329618000B03C70000000CF10000620BD800000020000000016F9E8A5700000C3A2478209A8DF80005013B09000B00170D0101020103010400B301B4003200330016034703F0011504EF01070902EF0A05CA0B04B04326C544003242329618000B03C70000000CF10000620BD800000020000200006E9C
13.01 12:54:25 [GPS] data from server (4) :00000002
.....
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

