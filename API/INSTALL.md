SECTION 1 - INSTALLING BLUEZ 4.99

	1) Get Bluez 4.99 (Compatible & Working/Tested in Debian 6)
	wget http://www.kernel.org/pub/linux/bluetooth/bluez-4.99.tar.xz 

	2) Look for installed bluez packages
	dpkg --get-selections | grep -v deinstall | grep bluez

		Possible Output: 	bluez
					bluez-obex
					...

	3) REMOVE ALL bluez packages listed above - NOTE: IF YOU INSTALLED BLUEZ PACKAGES WITH 'make install'(they are not listed in dpkg) PLEASE SEE NOTE1 BELOW 
		eg: 	apt-get --purge remove bluez
			apt-get --purge remove bluez-obex

	4) Reboot BBB
	reboot

	5) Install Dependencies
	sudo apt-get install libglib2.0-dev libdbus-1-dev libusb-dev libudev-dev libical-dev systemd libreadline-dev

	6) Extract Bluez 4.99
	tar xf bluez-4.99.tar.xz

	7) Change dir to bluez-4.99
	cd bluez-4.99

	8) Configure
	./configure

	9) Make and Install
	make && make install
	
	
SECTION 2 - COMPILING GeniAle ble_api

	1)	From the root folder of the ble_api : make
	
	2) 	chmod +x ble_api
	
	3) 	To test it: ./ble_api -n BlueTemp
	
	If it does not compile, install all bluez depencies and bluez-4.99 (see section 1)
	

	
NOTE1: Manually uninstalling bluez
If you've installed bluez with make install you need to follow the following steps:

1) Get in the directory from which you have made the make install
2) make uninstall

This should remove the version of bluez you have installed with make install