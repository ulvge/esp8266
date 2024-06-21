/*
 * 智能语言控制控制，支持天猫、小爱、小度、google Assistent同时控制
 * Time:20211127
 * Author: 2345VOR
 * 项目实例：发送on、off的指令开关灯
 * 参考文献：https://bbs.bemfa.com/84/last
 */
#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>

#define server_ip "bemfa.com" // 巴法云服务器地址默认即可
#define server_port "8344"    // 服务器端口，tcp创客云端口8344

//********************需要修改的部分*******************//

#define wifi_name "J09 502"                      // WIFI名称，区分大小写，不要写错
#define wifi_password "qwertyuiop111"            // WIFI密码
String UID = "e8882ae28d5bde39766c330ea913fd46"; // 用户私钥，可在控制台获取,修改为自己的UID
String TOPIC = "light002";                       // 主题名字，可在控制台新建
const int LED_Pin = 12;                          // 单片机LED引脚值，D2是NodeMcu引脚命名方式，其他esp8266型号将D2改为自己的引脚

const int buttonPin = 13;  // input
const int ledPinRed = 16;  // output
const int ledPinPink = 12; // output
const int outputPin = 14;  // output

String upUrl = "http://bin.bemfa.com/b/3BcYmQzODQ1NDdmZDdmNGJiMTllNGFiNGRiODM3YjFjNDc=switch001.bin"; // 固件链接，在巴法云控制台复制、粘贴到这里即可

//**************************************************//
// 最大字节数
#define MAX_PACKETSIZE 512
// 设置心跳值30s
#define KEEPALIVEATIME 30 * 1000
// tcp客户端相关初始化，默认即可
WiFiClient TCPclient;
String TcpClient_Buff = ""; // 初始化字符串，用于接收服务器发来的数据
unsigned int TcpClient_BuffIndex = 0;
unsigned long TcpClient_preTick = 0;
unsigned long preHeartTick = 0;    // 心跳
unsigned long preTCPStartTick = 0; // 连接
bool preTCPConnected = false;
// 相关函数初始化
// 连接WIFI
void doWiFiTick();

// TCP初始化连接
void doTCPClientTick();
void startTCPClient();
void sendtoTCPServer(String p);

// led控制函数，具体函数内容见下方
#define turnOnLed() digitalWrite(LED_Pin, LOW);
#define turnOffLed() digitalWrite(LED_Pin, HIGH);

// 初始化，相当于main 函数
void setup()
{
    Serial.begin(115200);

    pinMode(buttonPin, INPUT);

    pinMode(ledPinRed, OUTPUT);
    pinMode(ledPinPink, OUTPUT);
    setLed(LED_MODE_BLINK);

    pinMode(outputPin, OUTPUT);
    digitalWrite(outputPin, LOW);

    pinMode(LED_Pin, OUTPUT);
    digitalWrite(LED_Pin, HIGH);
    Serial.println("Beginning...");
}

// 循环
void loop()
{
    doWiFiTick();
    doTCPClientTick();
}