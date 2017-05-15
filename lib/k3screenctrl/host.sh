#!/bin/bash
# Copyright (C) 2017 XiaoShan https://www.mivm.cn

online_list=($(grep -v "0x0" /proc/net/arp | grep "br-lan" |awk '{print $1}'))
mac_online_list=($(grep -v "0x0" /proc/net/arp | grep "br-lan" |awk '{print $4}'))

arp_ip=($(grep "br-lan" /proc/net/arp | awk '{print $1}'))

if [ -z "$(iptables --list | grep UPSP)" -a -z "$(iptables --list | grep DWSP)" ]; then
	iptables -N UPSP
	iptables -N DWSP
	mkdir /tmp/lan_speed
fi
for ((i=0;i<${#arp_ip[@]};i++))
do
	if [ -z "$(iptables -nvx -L FORWARD | grep DWSP | grep ${arp_ip[i]})" -a -z "$(iptables -nvx -L FORWARD | grep UPSP | grep ${arp_ip[i]})" ]; then
		iptables -I FORWARD 1 -s ${arp_ip[i]} -j UPSP
		iptables -I FORWARD 1 -d ${arp_ip[i]} -j DWSP
		echo $(date +%s) > /tmp/lan_speed/${arp_ip[i]}
		echo 0 >> /tmp/lan_speed/${arp_ip[i]}
		echo 0 >> /tmp/lan_speed/${arp_ip[i]}
		up_sp[i]=0
		dw_sp[i]=0
	else
		last_speed_time=$(cut -d$'\n' -f,1 /tmp/lan_speed/${arp_ip[i]})
		last_speed_up=$(cut -d$'\n' -f,2 /tmp/lan_speed/${arp_ip[i]})
		last_speed_dw=$(cut -d$'\n' -f,3 /tmp/lan_speed/${arp_ip[i]})
		now_speed_time=$(date +%s)
		now_speed_up=$(iptables -nvx -L FORWARD | grep UPSP | grep ${arp_ip[i]}  | awk '{print $2}')
		now_speed_dw=$(iptables -nvx -L FORWARD | grep DWSP | grep ${arp_ip[i]}  | awk '{print $2}')
		time_s=$(($now_speed_time - $last_speed_time))
		#echo $time_s
		up_sp[i]=$((($now_speed_up - $last_speed_up) / $time_s))
		dw_sp[i]=$((($now_speed_dw - $last_speed_dw) / $time_s))
		echo $now_speed_time > /tmp/lan_speed/${arp_ip[i]}
		echo $now_speed_up >> /tmp/lan_speed/${arp_ip[i]}
		echo $now_speed_dw >> /tmp/lan_speed/${arp_ip[i]}
	fi
done

echo ${#online_list[@]}

for ((i=0;i<${#online_list[@]};i++))
do
	x=$((${#online_list[@]} - $i - 1))
	hostname=$(cat /tmp/dhcp.leases | grep ${online_list[i]} | awk '{print $4}')
	hostmac=${mac_online_list[i]//:/} && hostmac=${hostmac:0:6}
	logo=$(grep -n -i "$hostmac" /etc/oui/oui.txt) && logo=${logo:0:2} && logo=${logo//:/}
	if [ -z "$hostname" ]; then
		hostname="Unknown"
	fi
	if [ -z "$logo" ]; then
		logo="0"
	fi
	echo $hostname
	echo ${dw_sp[i]}
	echo ${up_sp[i]}
	echo $logo
done

echo 0
