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

7.5) Use custom gatttool
Copy the GIT/BlueZDev/Lib/gatttool.c     over      bluez-4.99/attrib/gatttool.c

8) Configure
./configure

9) Make and Install
make && make install

10) Manually Copy gatttool - NOTE: MAYBE NOT NECESSARY
cp attrib/gatttool /usr/local/bin

11) Manually Copy gattclient (GeniAle Specific) - *NOT IMPLEMENTED YET
cp attrib/gattclient /usr/local/bin



NOTE1: Manually uninstalling bluez
If you've installed bluez with make install you need to follow the following steps:

1) Get in the directory from which you have made the make install
2) make uninstall

This should remove the version of bluez you have installed with make install