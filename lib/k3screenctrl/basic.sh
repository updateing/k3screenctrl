#!/bin/sh
. /etc/os-release

PRODUCT_NAME_FULL=$(cat /etc/board.json | jsonfilter -e "@.model.name")
PRODUCT_NAME=${PRODUCT_NAME_FULL#* } # Remove first word to save space

WAN_IFNAME=$(uci get network.wan.ifname)
MAC_ADDR=$(ifconfig $WAN_IFNAME | grep -oE "([0-9A-Z]{2}:){5}[0-9A-Z]{2}")

CPU_TEMP=$(($(cat /sys/class/thermal/thermal_zone0/temp) / 1000))

HW_VERSION=${LEDE_DEVICE_REVISION:0:2}
FW_VERSION=${BUILD_ID:0:17}

echo $PRODUCT_NAME
echo $HW_VERSION $CPU_TEMP
echo $FW_VERSION
echo $MAC_ADDR
