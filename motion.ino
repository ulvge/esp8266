
 
#include "comm.h"
 // 定义Wi-Fi账号和密码
const char* wifiCredentials[][2] = {
  {"FAST_FFF43A", "1234567890"},
  {"5530", "123456780"},
  {"hxzy_guest", "hxzy123123!"}
};

const int wifiCount = sizeof(wifiCredentials) / sizeof(wifiCredentials[0]);

/*
 *检查数据，发送心跳
 */
void doTCPClientTick()
{
    // 检查是否断开，断开后重连
    if (WiFi.status() != WL_CONNECTED)
        return;
    if (!TCPclient.connected()) { // 断开重连
        if (preTCPConnected == true) {
            preTCPConnected = false;
            preTCPStartTick = millis();
            Serial.println();
            Serial.println("TCP Client disconnected.");
            TCPclient.stop();
        } else if (millis() - preTCPStartTick > 1 * 1000) // 重新连接
            startTCPClient();
    } else {
        if (TCPclient.available()) { // 收数据
            char c = TCPclient.read();
            TcpClient_Buff += c;
            TcpClient_BuffIndex++;
            TcpClient_preTick = millis();

            if (TcpClient_BuffIndex >= MAX_PACKETSIZE - 1) {
                TcpClient_BuffIndex = MAX_PACKETSIZE - 2;
                TcpClient_preTick = TcpClient_preTick - 200;
            }
            preHeartTick = millis();
        }
        if (millis() - preHeartTick >= KEEPALIVEATIME) { // 保持心跳
            preHeartTick = millis();
            Serial.println("--Keep alive:");
            sendtoTCPServer("ping\r\n"); // 发送心跳，指令需\r\n结尾，详见接入文档介绍
        }
    }
    if ((TcpClient_Buff.length() >= 1) && (millis() - TcpClient_preTick >= 200)) {
        TCPclient.flush();
        Serial.print("Rev string: ");
        TcpClient_Buff.trim();          // 去掉首位空格
        Serial.println(TcpClient_Buff); // 打印接收到的消息
        String getTopic = "";
        String getMsg = "";
        if (TcpClient_Buff.length() > 15) { // 注意TcpClient_Buff只是个字符串，在上面开头做了初始化 String TcpClient_Buff = "";
            // 此时会收到推送的指令，指令大概为 cmd=2&uid=xxx&topic=light002&msg=off
            int topicIndex = TcpClient_Buff.indexOf("&topic=") + 7;    // c语言字符串查找，查找&topic=位置，并移动7位，不懂的可百度c语言字符串查找
            int msgIndex = TcpClient_Buff.indexOf("&msg=");            // c语言字符串查找，查找&msg=位置
            getTopic = TcpClient_Buff.substring(topicIndex, msgIndex); // c语言字符串截取，截取到topic,不懂的可百度c语言字符串截取
            getMsg = TcpClient_Buff.substring(msgIndex + 5);           // c语言字符串截取，截取到消息
            Serial.print("topic:------");
            Serial.println(getTopic); // 打印截取到的主题值
            Serial.print("msg:--------");
            Serial.println(getMsg); // 打印截取到的消息值
        }
        if (getMsg == "on") { // 如果收到指令on==打开灯
            digitalWrite(outputPin, HIGH);
        } else if (getMsg == "off") { // 如果收到指令off==关闭灯
            digitalWrite(outputPin, LOW);
        } else if (getMsg == "update") { // 如果收到指令update
            updateBin();                 // 执行升级函数
        }

        TcpClient_Buff = "";
        TcpClient_BuffIndex = 0;
    }
}

/**************************************************************************
                                 WIFI
***************************************************************************/

//连接wifi
void setup_wifi() {
  delay(10);

  while(1){
    for (int i = 0; i < wifiCount; i++) {
      Serial.printf("Connecting to %s\n", wifiCredentials[i][0]);
      
      WiFi.begin(wifiCredentials[i][0], wifiCredentials[i][1]);

      delay(1000);
      //等待WiFi连接
      if (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
      } else{
          //wifi连接成功后输出成功信息
          Serial.println("");
          Serial.println("WiFi Connected!");   //显示wifi连接成功
          Serial.println(WiFi.localIP());       //返回wifi分配的IP
          Serial.println(WiFi.macAddress());  //返回设备的MAC地址
          Serial.println(""); 
          randomSeed(micros());
          return;
      }
    }
  }
}
/*
  WiFiTick
  检查是否需要初始化WiFi
  检查WiFi是否连接上，若连接成功启动TCP Client
  控制指示灯
*/
void doWiFiTick()
{
    static bool startSTAFlag = false;
    static bool taskStarted = false;

    if (!startSTAFlag) {
        startSTAFlag = true;
        WiFi.disconnect();
        WiFi.mode(WIFI_STA);
    }

    // 未连接1s重连
    if (WiFi.status() != WL_CONNECTED) {
        setup_wifi();
    }else{
        // 连接成功之后
        startTCPClient();
    }
}

int g_ledMode = LED_MODE_OFF;

// cmd enable disable. 
void setLed(int mode){
  static int state = 0;
  static int blinkCount = 0;
  static int enable = 1;
  switch(mode){
    case LED_MODE_EN:
        enable = 1;
        break;
    case LED_MODE_DIS:
        enable = 0;
    case LED_MODE_OFF:
        digitalWrite(ledPinRed, LOW);
        digitalWrite(ledPinPink, LOW);
        break;
    case LED_MODE_ON_1:
      if (!enable) break;
      digitalWrite(ledPinRed, HIGH);
    break;
    case LED_MODE_OFF_1:
      if (!enable) break;
      digitalWrite(ledPinRed, LOW);
    break;
    case LED_MODE_ON_2:
      if (!enable) break;
      digitalWrite(ledPinPink, HIGH);
    break;
    case LED_MODE_OFF_2:
      if (!enable) break;
      digitalWrite(ledPinPink, LOW);
    break;
    case LED_MODE_ON_ALL:
      if (!enable) break;
      digitalWrite(ledPinRed, HIGH);
      digitalWrite(ledPinPink, HIGH);
    case LED_MODE_BLINK:
      if (blinkCount++ > 100){
        blinkCount = 0;
        state = !state;
        digitalWrite(ledPinRed, state);
        digitalWrite(ledPinPink, state);
      }
    break;
  }
  g_ledMode = mode;
}


const unsigned int debounceDelay = 30;
unsigned int lastDebounceTime;
void monitorButton() {
  // read the state of the pushbutton value:
  static int buttonStateLast = 0;
  static int isChanged = 0;
  int buttonState = digitalRead(buttonPin);
// 检查是否需要去抖（按键状态是否发生变化）
  if (buttonState == LOW){
    if (buttonState != buttonStateLast) { // 刚按下，记录当前时间
      isChanged = 0;
      lastDebounceTime = millis(); // 记录上次变化的时间
    }else{// 一直被按下
      if ((millis() - lastDebounceTime) >= debounceDelay) {//满足去抖条件
        if (isChanged == 0){
          isChanged = 1;
          int newState = !digitalRead(outputPin);
          digitalWrite(outputPin, newState);
          setLed(newState ? LED_MODE_ON_1 : LED_MODE_OFF_1);
          Serial.printf("Button pressed, now outputPin state is %d\n", newState);
        }
      }
    }
  }else{
    isChanged = 0;
  }
  buttonStateLast = buttonState; // update
}

