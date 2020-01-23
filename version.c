//const char *Version = "0.2 (19.07.2018)";//second version
//const char *Version = "0.3 (19.07.2018)";//command - first step
//const char *Version = "0.4 (20.07.2018)";//command - second step
//const char *Version = "0.5 (20.07.2018)";//command - full parse 'setdigout ABCD AT BT CT DT' command
//const char *Version = "0.6 (20.07.2018)";//multi samples in one packet (from 1 to 4) !!!!
//const char *Version = "0.7 (23.07.2018)";//write log to file, get imei aka second param when starting program (start/stop script present)
//const char *Version = "0.8 (23.07.2018)";//add show Ignition (DIN1) status change
//const char *Version = "0.8.1 (23.07.2018)";//New files sources + config file support (.virt.conf)
//const char *Version = "0.9 (24.07.2018)";//Add support new command - getgps, SET_ON, SET_OFF
//const char *Version = "1.0 (24.07.2018)";//Minor changes : done release for mode=0 - parking  +  add param config=config_file_name by start program
//const char *Version = "1.1 (25.07.2018)";//Minor changes in start mirror params for parking mode (mode=0)
//const char *Version = "1.2 (25.07.2018)";//Major changes : first move mode - step one (mode=1)
//const char *Version = "1.3 (26.07.2018)";//Major changes : "Бибика поехала..."
//const char *Version = "1.4 (27.07.2018)";//Minor changes in : move mode, configuration file
//const char *Version = "1.5 (27.07.2018)";//Minor changes : add wait_before_new_connect param. to configuration file
//const char *Version = "1.6 (27.07.2018)";//Minor changes : new name for log file (remove dot in first position)
//const char *Version = "1.7 (27.07.2018)";//Minor changes : new name pid files for each proc
//const char *Version = "1.8 (30.07.2018)";//Minor changes : add new direction for move mode
//const char *Version = "1.9 (31.07.2018)";//Minor changes in make_record() function : fuel_tank and sattelites emulator
//const char *Version = "2.0 (01.08.2018)";//Minor changes : set first location in configuration file and goto stop mode by event
//const char *Version = "2.1 (01.08.2018)";//Minor changes in main
//const char *Version = "2.2 (02.08.2018)";//Minor changes : start move from STOP if Ignition ON (via PARK mode), new data from ДУТ (AIN1)
//const char *Version = "2.3 (02.08.2018)";//Minor changes : clear odometer value for STOP or PARK scena
//const char *Version = "2.4 (03.08.2018)";//Major changes in main thread (faza2 + faza3 => faza2)
//const char *Version = "2.5 (03.08.2018)";//Major changes in move mode (start move by cmd unlock fuelpomp)
//const char *Version = "3.0 (11.01.2020)";//Major changes : first release for ESP32
//const char *Version = "3.1 (11.01.2020)";//Major changes : add sdcard (spi mode) support (first step)
//const char *Version = "3.2 (13.01.2020)";//Major changes : read config file 'conf.txt' from sdcard
//const char *Version = "3.3 (13.01.2020)";//Minor changes : add leds for next events - log_server start, connect to cars_server
//const char *Version = "3.4 (14.01.2020)";//Minor changes : add partition /disk with fatfs on dataflash
//const char *Version = "3.4.1 (14.01.2020)";//Minor changes : copy /sdcard/conf.txt to /spiflash/conf.txt (add read/write functions)
//const char *Version = "3.4.2 (14.01.2020)";//Minor changes : add delete all files GPIO_DELIT_PIN for /spiflash
//const char *Version = "3.5 (16.01.2020)";//Minor changes : add new command (restart device)
//const char *Version = "3.6 (16.01.2020)";//Minor changes : add mutex for ssd1306
//const char *Version = "3.7 (16.01.2020)";//Major changes : add ftp client (first step)
//const char *Version = "3.8 (17.01.2020)";//Major changes : save configuration file from ftp_server to /spiflash/conf.txt
//const char *Version = "3.9 (19.01.2020)";//Minor changes : add to NVS_FLASH param's for ftp server
//const char *Version = "4.0 (20.01.2020)";//Minor changes : add mutex to print_msg() function
//const char *Version = "4.1 (21.01.2020)";//Major changes : add tls_server task with control of device (11 ctrl commands)
//const char *Version = "4.2 (22.01.2020)";//Minor changes in tls_server
//const char *Version = "4.3 (22.01.2020)";//Major changes in tls_server : for ctrl_command 'get' add 8 sub_ctrl_commands
//const char *Version = "4.4 (23.01.2020)";//Minor changes in tls_server : add sub_ctrl_command 'gps_srv'
const char *Version = "4.5 (23.01.2020)";//Minor changes : in fmb630_client add display latitude and longitude on the ssd1306

