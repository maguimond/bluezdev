#!/bin/bash

# Arg1 is BLE address (tip: use hcitool lescan to get it).
BLE_ADDR=$1

if [ "$BLE_ADDR" ]
    then
        echo "BLE address found: "$BLE_ADDR
        # Enable notification and indication flags
        gatttool -b $BLE_ADDR --char-write-req -a 0x0011 -n 0300; gatttool -b $BLE_ADDR --char-write-req -a 0x000E -n A03E0300012C00648D; gatttool -b $BLE_ADDR --char-read --handle=0x0010 
else
    echo "Cannot get BLE address."
fi

