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
	fi
	#判断设备流量监视规则是否存在，不存在则添加
done

echo ${#online_list[@]}
#设备在线总数

for ((i=0;i<${#online_list[@]};i++))
do
	hostname=$(cat /tmp/dhcp.leases | grep ${online_list[i]} | awk '{print $4}')
	hostmac=${mac_online_list[i]//:/} && hostmac=${hostmac:0:6}
	logo=$(grep -n -i "$hostmac" /etc/oui/oui.txt) && logo=${logo:0:2} && logo=${logo//:/}
	#oui 下载地址:https://cdn.mivm.cn/OpenWrt/k3screenctrl/oui.txt
	#oui md5 校验:https://cdn.mivm.cn/OpenWrt/k3screenctrl/oui.txt.md5
	#oui 来源:斐讯官方 + 个人整理 + 用户反馈

	last_speed_time=$(cut -d$'\n' -f,1 /tmp/lan_speed/${online_list[i]})
	last_speed_up=$(cut -d$'\n' -f,2 /tmp/lan_speed/${online_list[i]})
	last_speed_dw=$(cut -d$'\n' -f,3 /tmp/lan_speed/${online_list[i]})
	now_speed_time=$(date +%s)
	now_speed_up=$(iptables -nvx -L FORWARD | grep UPSP | grep ${online_list[i]}  | awk '{print $2}')
	now_speed_dw=$(iptables -nvx -L FORWARD | grep DWSP | grep ${online_list[i]}  | awk '{print $2}')
	time_s=$(($now_speed_time - $last_speed_time))
	if [ $time_s -eq 0 ];then
		time_s=1
	fi
	up_sp=$((($now_speed_up - $last_speed_up) / $time_s))
	dw_sp=$((($now_speed_dw - $last_speed_dw) / $time_s))
	echo $now_speed_time > /tmp/lan_speed/${arp_ip[i]}
	echo $now_speed_up >> /tmp/lan_speed/${arp_ip[i]}
	echo $now_speed_dw >> /tmp/lan_speed/${arp_ip[i]}
	#依照前后相差的时间差来计算出平均速度，跟 WAN.sh 原理一样

	if [ -z "$hostname" ]; then
		hostname="Unknown"
	fi
	if [ -z "$logo" ]; then
		logo="0"
	fi
	echo $hostname
	echo $dw_sp
	echo $up_sp
	echo $logo
done

last_arp_refresh_time=$(cat /tmp/arp_refresh_time)
now_arp_refresh_time=$(date +%s)
if [ -z "$last_arp_refresh_time" ]; then
	echo $now_arp_refresh_time > /tmp/arp_refresh_time
elif [ $(($now_arp_refresh_time - $last_arp_refresh_time)) -ge 600 ]; then
	for ((i=0;i<${#online_list[@]};i++))
	do
		arp -d ${online_list[i]}
	done
	echo $now_arp_refresh_time > /tmp/arp_refresh_time
fi
#设备异常断开后，ARP路由表不会更新设备状态，所以每10分钟彻底更新一次路由表状态，需要ARP二进制的支持，自定义编译BusyBox即可。

echo 0
#屏幕控制程序的一个BUG，最后不输出一行就显示错误，不知道是什么鬼

