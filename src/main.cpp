#include <Arduino.h>
#include "WiFi.h"
#include "XFS.h" //封装好的命令库
#include "123.h" //中文需要放在该记事本中（因为编码不兼容）
#include <esp_now.h>
#include <Wire.h>
#include <U8g2lib.h>

/* 引脚参数 */
#define XFS_SDA 21
#define XFS_SCL 22
#define OLED_SDA 18
#define OLED_SCL 23

/* 双路I2C */
TwoWire I2C_XFS = TwoWire(0);
TwoWire I2C_OLED = TwoWire(1);

/* Oled 实例化 */
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/OLED_SCL, /* data=*/OLED_SDA, /* reset=*/U8X8_PIN_NONE);  // SDA:21 scl:22
const char* state = "FxxK";
String macAddr;

/*实例化语音合成对象*/
XFS5152CE xfs;

/*超时设置，示例为30S*/
static uint32_t LastSpeakTime = 0;
#define SpeakTimeOut 10000

/**
    @brief  初始化语音合成
    @param  无
    @retval 无
*/
static void XFS_Init()
{
  uint8_t n = 5;
  Serial.println("初始化语音播报模块");
  xfs.Begin(0x30); // 设备i2c地址，地址为0x50
  delay(n);
  Serial.println("设置发音人");
  xfs.SetReader(XFS5152CE::Reader_XiaoYan); // 设置发音人
  delay(n);
  Serial.println("文本的编码格式");
  xfs.SetEncodingFormat(XFS5152CE::GB2312); // 文本的编码格式
  delay(n);
  Serial.println("语种判断");
  xfs.SetLanguage(xfs.Language_Auto); // 语种判断
  delay(n);
  Serial.println("合成风格设置");
  xfs.SetStyle(XFS5152CE::Style_Continue); // 合成风格设置
  delay(n);
  Serial.println("设置单词的发音方式");
  xfs.SetArticulation(XFS5152CE::Articulation_Letter); // 设置单词的发音方式
  delay(n);
  Serial.println("设置语速");
  xfs.SetSpeed(5); // 设置语速1~10
  delay(n);
  Serial.println("设置语调");
  xfs.SetIntonation(5); // 设置语调1~10
  delay(n);
  Serial.println("设置音量");
  xfs.SetVolume(5); // 设置音量1~10
  delay(n);
}

/**
    @brief  语音合成播报
    @param  int index
    @retval 无
*/
void broadcast(int index){
  xfs.SetReader(XFS5152CE::Reader_XuJiu); // 设置发音人;
  xfs.StartSynthesis(TextTab2[index]);
  while (xfs.GetChipStatus() != xfs.ChipStatus_Idle)
  {
    delay(30);
  }
}

//获取接收器mac地址
void getMac()
{
  WiFi.mode(WIFI_MODE_STA);
  macAddr = WiFi.macAddress();
  Serial.println(macAddr);
  printOLED(macAddr.c_str(), 0);
}

//在OLED上打印字符串
void printOLED(const char* str, int x = 60, int y = 40){
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_wqy15_t_gb2312);
    u8g2.drawUTF8(x, y, str);
  } while (u8g2.nextPage());
  delay(500);
}

unsigned char result = 0xFF;
void setup()
{
  Serial.begin(115200); // 串口波特率设置，打印数据时串口需要选择和这里一样的波特率

  //设置双路I2C引脚参数
  I2C_XFS.setPins(XFS_SDA, XFS_SCL);
  I2C_OLED.setPins(OLED_SDA, OLED_SCL);

  //启动IIC总线
  I2C_XFS.begin();
  I2C_OLED.begin();

  delay(100);

  // 屏幕初始化
  u8g2.begin();
  u8g2.enableUTF8Print();

  // 语音合成初始化
  XFS_Init();
  broadcast(0);
  
  //获取接收器mac地址
  getMac();

  delay(500);

  state = "工作中";
  printOLED(state,20);

}
void loop()
{
  
  
}