#/bin/sh
wget https://cdn.mivm.cn/OpenWrt/k3screenctrl/oui.txt.md5 -O /tmp/oui.txt.md5
if [ -s "/tmp/oui.txt.md5" ]; then
	md5=$(cat /tmp/oui.txt.md5 | awk '{print $2}' | tr '[A-Z' '[a-z]')
	echo $md5
	md5sum=$(md5sum /etc/oui/oui.txt | awk '{print $1}')
	echo $md5sum
	if [ "$md5" != "$md5sum" ]; then
		wget https://cdn.mivm.cn/OpenWrt/k3screenctrl/oui.txt -O /tmp/oui.txt
		if [ -s "/tmp/oui.txt" ]; then
			logger 屏幕设备图标 MAC 识别库 oui.txt 更新完成
			mv /tmp/oui.txt /etc/oui/oui.txt
		fi
	fi
else
	logger 屏幕设备图标 MAC 识别库 oui.txt 校验文件下载失败 无法更新
fi
