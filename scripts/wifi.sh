#!/bin/sh

print_wifi_info() {
    local ucidev=$1 ifname=$2
    local complete_stat status enabled ssid psk client_count

    complete_stat=`wifi status`
    status=`echo $complete_stat | jsonfilter -e "@.$ucidev.disabled"`
    if [ "x$status" == "xtrue" -o -z "`echo $complete_stat | jsonfilter -e "@.$ucidev"`" ]; then
        ssid=
        psk=
        client_count=0
        enabled=0
    elif [ "x$status" == "xfalse" ]; then
        ssid=`echo $complete_stat | jsonfilter -e "@.$ucidev.interfaces[0].config.ssid"`
        psk=`echo $complete_stat | jsonfilter -e "@.$ucidev.interfaces[0].config.key"`
        client_count=`iw dev $ifname station dump | grep Station | wc -l`
        enabled=1
    fi

    echo $ssid
    echo $psk
    echo $enabled
    echo $client_count
}

echo 0 # Band mix
print_wifi_info radio0 wlan0 # 2.4GHz
print_wifi_info radio1 wlan1 # 5GHZ
print_wifi_info radiox wlanx # Visitor - not implemented