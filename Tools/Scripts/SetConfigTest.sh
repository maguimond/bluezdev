#!/bin/bash

# Get address from bluecomm app
BLE_ADDR=$(bluecomm | grep "BlueMon" | awk '{print $1;exit;}')

# SET_CONFIG test:
# <CONFIG><SET><TxOption><TxPower><AnalogSensor1><AnalogSensor2><AnalogSensor3><AnalogSensor4><AnalogSensor5><AnalogSensor6><PowerMode><CHECKSUM>
#                  |
#      |--------------------------|
#     <7:TxState><6:TxWeigth><5..0:TxRate>
# TxState = 1
# TxWeigth = 0
# TxRate = 1
# TxOption = (TxState << 7) | (TxWeigth << 6) | (TxRate & 0x3F)
# TxPower = 0x00
# AnalogSensor1 = 0x01
# AnalogSensor2 = 0x00
# AnalogSensor3 = 0x00
# AnalogSensor4 = 0x00
# AnalogSensor5 = 0x00
# AnalogSensor6 = 0x00
# PowerMode = 0x00
# Checksum = ~(DataBytesSum & 0x00FF) = 0x9F
# Packet result: 0xA03E8100010000000000009F

if [ "$BLE_ADDR" ]
    then
        echo "BLE address found: "$BLE_ADDR
        # Enable notification and indication flags
        gatttool -b $BLE_ADDR --char-write-req -a 0x0011 -n 0300
        # Send data packet through the BlueMon Request characteristic
        gatttool -b $BLE_ADDR --char-write-req -a 0x000E -n A03E8100010000000000009F
        # Listen to the BlueMon Reply characteristic
        gatttool -b $BLE_ADDR --char-read --handle=0x0010 --listen
else
    echo "Cannot get BLE address."
fi

