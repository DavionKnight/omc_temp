all:
	make -C ../common
	make -C init 
	make -C ../fun
	make -C ../drv/iic_cpld 
	make -C ../drv/iic_laser 
	make -C ../drv/led
	make -C ../drv/gpio_int
	make -C ../cgi
	cp init/cm_gsm_mu_p6 obj
	cp ../fun/fun obj
	cp ../drv/iic_cpld/iic_cpld.ko obj
	cp ../drv/iic_laser/iic_laser.ko obj
	cp ../drv/led/led.ko obj
	cp ../drv/gpio_int/gpio_int.ko obj
	cp ../cgi/dru.cgi obj
	cp boardinit.sh obj
clean:
	make clean -C ../common
	make clean -C init
	make clean -C ../fun
	make clean -C ../drv/iic_cpld 
	make clean -C ../drv/iic_laser 
	make clean -C ../drv/led 
	make clean -C ../drv/gpio_int 
	make clean -C ../cgi
	rm -rf ./obj/*
