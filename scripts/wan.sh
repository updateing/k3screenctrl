#!/bin/sh

COREUTILS_DATE=`/usr/bin/date`
TEMP_FILE="/tmp/wan_speed_temp"
WAN_IFNAME=`uci get network.wan.ifname`
IPV4_ADDR=`ip -4 addr show dev $WAN_IFNAME scope global`
IPV6_ADDR=`ip -6 addr show dev $WAN_IFNAME scope global`

if [ -n "$IPV4_ADDR" -o -n "$IPV6_ADDR" ]; then
    CONNECTED=1
else
    CONNECTED=0
fi

CURR_TIME=$($COREUTILS_DATE +%s%N)
CURR_STAT=$(cat /proc/net/dev | grep $WAN_IFNAME | sed -e 's/^ *//' -e 's/  */ /g')
CURR_DOWNLOAD_BYTES=$(echo $CURR_STAT | cut -d " " -f 2)
CURR_UPLOAD_BYTES=$(echo $CURR_STAT | cut -d " " -f 10)

LINENO=0
while read line; do
    case "$LINENO" in
        0)
            LAST_TIME=$line
            ;;
        1)
            LAST_UPLOAD_BYTES=$line
            ;;
        2)
            LAST_DOWNLOAD_BYTES=$line
            ;;
        *)
            ;;
    esac
    LINENO=$(($LINENO+1))
done < $TEMP_FILE

echo $CURR_TIME > $TEMP_FILE
echo $CURR_UPLOAD_BYTES >> $TEMP_FILE
echo $CURR_DOWNLOAD_BYTES >> $TEMP_FILE

TIME_DELTA_S=$((($CURR_TIME-$LAST_TIME)/1000000000))
if [ $TIME_DELTA_S -eq 0 ]; then
    TIME_DELTA_S=1
fi
UPLOAD_BPS=$((($CURR_UPLOAD_BYTES-$LAST_UPLOAD_BYTES)/$TIME_DELTA_S))
DOWNLOAD_BPS=$((($CURR_DOWNLOAD_BYTES-$LAST_DOWNLOAD_BYTES)/$TIME_DELTA_S))

echo $CONNECTED
echo $UPLOAD_BPS
echo $DOWNLOAD_BPS
