#!/bin/bash

# Initialise le device bluetooth
echo "Remise à zéro du device"
if [ $(hciconfig hci0 reset) ]; then
	exit 1;
fi

# Get address from bluecomm app
echo "Scan des devies bluetooth"
#BLE_ADDR=$(hcitool lescan | grep "BlueTemp" | awk '{print $1}')
BLE_ADDR='C5:2A:45:36:E3:A2'

# GET_SENSOR_DATA test:
# <SENSOR_DATA><GET><CHECKSUM>
# Checksum = ~(DataBytesSum & 0x00FF) = 0x22
# Packet result: 0xA13C22

if [ $BLE_ADDR ]; then
        echo "BLE address found: "$BLE_ADDR
        # Enable notification and indication flags
        gatttool -b $BLE_ADDR --char-write-req -a 0x0011 -n 0300
        # Send data packet through the BlueMon Request characteristic
        gatttool -b $BLE_ADDR --char-write-req -a 0x000E -n A13C22
        # Listen to the BlueMon Reply characteristic
	DATA=$(gatttool -b $BLE_ADDR --char-read --handle=0x0010 --listen)

else
    echo "Cannot get BLE address."
fi

