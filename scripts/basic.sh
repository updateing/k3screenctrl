#!/bin/sh
. /etc/os-release

WAN_IFNAME=$(uci get network.wan.ifname)
PRODUCT_NAME_FULL=$(cat /etc/board.json | jsonfilter -e "@.model.name")
PRODUCT_NAME=${PRODUCT_NAME_FULL#* } # Remove first word to save space
HW_VERSION=$(nvram get hd_version)
FW_VERSION=${BUILD_ID:0:17}
MAC_ADDR=$(ifconfig $WAN_IFNAME | grep -oE "([0-9A-Z]{2}:){5}[0-9A-Z]{2}")

echo $PRODUCT_NAME
echo $HW_VERSION
echo $FW_VERSION
echo $MAC_ADDR