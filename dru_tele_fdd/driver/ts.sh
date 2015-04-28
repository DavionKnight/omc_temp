#!/bin/bash
tftp -g -r dru_object 192.168.10.81
tftp -g -r fpga.rbf 192.168.10.81
chmod +x /ramDisk/dru_object
#source /ramDisk/dru_object
./dru_object
