#!/bin/sh
file=dru_cm_dcs
{
	sleep 1
	echo "root"
	sleep 1
	echo "killall $file"
	sleep 1
} | telnet $1 

ftp $1 <<END_FTP
usr root
put obj/$file $file 
END_FTP
{
	sleep 1
	echo "root"
	sleep 1
	echo "reboot"
	sleep 1
} | telnet $1 

