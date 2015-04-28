#!/bin/sh
RUNPROGRAM="dru"
echo "******Setup IP Address******"
/sbin/ifconfig lo 127.0.0.1 up netmask 255.0.0.0
/sbin/ifconfig eth0 192.168.10.110 netmask 255.255.255.0
/sbin/route add default gw 192.168.10.1

mkdir /lib/modules/$(uname -r)
echo /sbin/mdev > /proc/sys/kernel/hotplug
mkdir /mnt/app
mkdir /mnt/app/boa
mkdir /mnt/app/boa/www
mkdir /mnt/app/boa/www/cgi-bin
cp /flashDev/program/web/* /mnt/app/boa/www -r
cp /flashDev/program/dru.cgi /mnt/app/boa/www/cgi-bin
chmod 777 /mnt/app/*
chmod 777 /mnt/app/boa/*
chmod 777 /mnt/app/boa/www/*
chmod 777 /mnt/app/boa/www/cgi-bin/*
boa

echo "******telnetd start******"
/usr/sbin/telnetd start
tcpsvd -vE 0.0.0.0 21 ftpd -w /flashDev/program/ &

if (test ! -d /flashDev/data/log)
then
  mkdir /flashDev/data/log
fi
if (test ! -d /flashDev/program/config)
then
  mkdir /flashDev/program/config
fi
tar zxvf /flashDev/program/dru_pkg.tar.gz -C /ramDisk/
chmod +x /flashDev/program/*
#等待3s,如果有按键输入不允许控制程序
read -t 3 -p "Do you want run program [y/n]?:" answer
case $answer in
  N|n)
  echo -E
  ;;
  *)
  echo "******Run App******"
  cp /flashDev/program/fun /usr/bin
  chmod 777 /usr/bin/fun
  insmod /flashDev/program/iic_cpld.ko
  insmod /flashDev/program/iic_laser.ko
  insmod /flashDev/program/led.ko
  insmod /flashDev/program/gpio.ko
  /ramDisk/$RUNPROGRAM &
  ;;
esac
