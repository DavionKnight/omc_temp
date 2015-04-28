#!/bin/sh
file=dru_um_dcs
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

