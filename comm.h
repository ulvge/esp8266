#ifndef COMM_H
#define COMM_H


typedef enum {
  LED_MODE_OFF,
  LED_MODE_ON_1,
  LED_MODE_ON_2,
  LED_MODE_OFF_1,
  LED_MODE_OFF_2,
  LED_MODE_BLINK,
  LED_MODE_ON_ALL,
  LED_MODE_EN,
  LED_MODE_DIS
} LED_MODE;


//********************需要修改的部分*******************//
#define server_ip "bemfa.com" // 巴法云服务器地址默认即可
#define server_port "8344"    // 服务器端口，tcp创客云端口8344

String UID = "bd384547fd7f4bb19e4ab4db837b1c47"; // 用户私钥，可在控制台获取,修改为自己的UID
String TOPIC = "switch001";                       // 主题名字，可在控制台新建
const String g_updateURL = "http://bin.bemfa.com/b/3BcYmQzODQ1NDdmZDdmNGJiMTllNGFiNGRiODM3YjFjNDc=switch001.bin"; // 固件链接，在巴法云控制台复制、粘贴到这里即可

#endif