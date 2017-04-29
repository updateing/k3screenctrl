#!/bin/sh
. /etc/os-release

WAN_IFNAME=$(uci get network.wan.ifname)
PRODUCT_NAME=$(cat /etc/board.json | jsonfilter -e "@.model.name")
HW_VERSION=A1
FW_VERSION=${BUILD_ID:0:17}
MAC_ADDR=$(ifconfig $WAN_IFNAME | grep -oE "([0-9A-Z]{2}:){5}[0-9A-Z]{2}")

echo $PRODUCT_NAME
echo $HW_VERSION
echo $MAC_ADDR
echo $FW_VERSION