EA:6A:97:5D:C3:F0#!/bin/bash

# Initialise le device bluetooth
#echo "Remise à zéro du device"
#if [ $(hciconfig hci0 reset) ]; then
#	exit 1;
#fi

BLE_ADDR=$1

# GET_SENSOR_DATA test:
# <SENSOR_DATA><GET><CHECKSUM>
# Checksum = ~(DataBytesSum & 0x00FF) = 0x22
# Packet result: 0xA13C22

if [ $BLE_ADDR ]; then
        echo "BLE address found: "$BLE_ADDR
        # Enable notification and indication flags
        gatttool -b $BLE_ADDR --char-write-req -a 0x0011 -n 0300
        # Send data packet through the BlueMon Request characteristic
        gatttool -b $BLE_ADDR --char-write-req -a 0x000E -n A13C22 --listen
else
    echo "Cannot get BLE address."
fi
