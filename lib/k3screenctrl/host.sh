#!/bin/bash
# Copyright (C) 2017 XiaoShan https://www.mivm.cn

online_list=($(cat /proc/net/arp |grep "0x2" |awk '{print $1}'))


cat /proc/net/arp | grep "0x2" | awk '{print $1}' > /tmp/arp_ip
iptables -N UPLOADSPEED
iptables -N DOWNLOADSPEED
while read line;do iptables -I FORWARD 1 -s $line -j UPLOADSPEED;done < /tmp/arp_ip
while read line;do iptables -I FORWARD 1 -d $line -j DOWNLOADSPEED;done < /tmp/arp_ip
sleep 1
up_sp=($(iptables -nvx -L FORWARD | grep DOWNLOADSPEED | awk '{print $2}'))
dw_sp=($(iptables -nvx -L FORWARD | grep UPLOADSPEED | awk '{print $2}'))
while read line;do iptables -D FORWARD -s $line -j UPLOADSPEED;done < /tmp/arp_ip
while read line;do iptables -D FORWARD -d $line -j DOWNLOADSPEED;done < /tmp/arp_ip
iptables -X UPLOADSPEED
iptables -X DOWNLOADSPEED

echo "${#online_list[@]}"

for ((i=0;i<${#online_list[@]};i++))
do
	x=$(expr "${#online_list[@]}" - "$i" - "1")
	homename=$(cat /tmp/dhcp.leases | grep ${online_list[i]} | awk '{print $4}')
	echo $homename
	echo ${up_sp[x]}
	echo ${dw_sp[x]}
	echo 0
done
echo 0
