项目功能：
	插座可实现远程遥控和定时
项目背景：
	天猫精灵插座只能手动开关，无法实现定时功能，于是重新刷入代码

硬件信息:
	吉控智能插座，YKYC-001。
	
	C:\Users\admin>esptool.py --port COM9 chip_id
	esptool.py v3.0
	Serial port COM9
	Connecting...
	Detecting chip type... ESP8266
	Chip is ESP8285
	Features: WiFi, Embedded Flash
	Crystal is 26MHz
	MAC: 84:0d:8e:41:95:13
	Uploading stub...
	Running stub...
	Stub running...
	Chip ID: 0x00419513
	Hard resetting via RTS pin...
	
	

软件信息：
	arduino + 巴法云 + mqtt
	
	https://cloud.bemfa.com/tcp/index.html
	
	灯的逻辑:
	 上电未连接wifi时init: LED_MODE_BLINK
// button: LED_MODE_BLUE_OFF; LED_MODE_RED_ON/OFF
// msg:  LED_MODE_BLUE_ON/OFF; LED_MODE_RED_OFF



烧录方式：
	需要IO0拉低的同时，再把RST也拉低，进行复位，此时芯片会进入下载模式，并一直等待烧录命令
	需要IO0拉低的同时，再把RST也拉低，进行复位，此时芯片会进入下载模式，并一直等待烧录命令
	需要IO0拉低的同时，再把RST也拉低，进行复位，此时芯片会进入下载模式，并一直等待烧录命令
	
也可用esptool烧录
	安装烧录工具：
	pip install esptool
	esptool.py --port COM3 erase_flash
	esptool.py --port COM3 --baud 115200 write_flash --flash_size=detect  0 1471.bin