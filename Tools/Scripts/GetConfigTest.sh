#!/bin/bash

# Get address from bluecomm app
BLE_ADDR=$(bluecomm | grep "BlueMon" | awk '{print $1;exit;}')

# GET_CONFIG test:
# <CONFIG><GET><CHECKSUM>
# Checksum = ~(DataBytesSum & 0x00FF) = 0x23
# Packet result: 0xA03C23

if [ "$BLE_ADDR" ]
    then
        echo "BLE address found: "$BLE_ADDR
        # Enable notification and indication flags
        gatttool -b $BLE_ADDR --char-write-req -a 0x0011 -n 0300
        # Send data packet through the BlueMon Request characteristic
        gatttool -b $BLE_ADDR --char-write-req -a 0x000E -n A03C23
        # Listen to the BlueMon Reply characteristic
        gatttool -b $BLE_ADDR --char-read --handle=0x0010 --listen
else
    echo "Cannot get BLE address."
fi

