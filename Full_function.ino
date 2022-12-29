#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <math.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>


#define TEMPHUMIDSENSOR_PIN 4   
#define LIGHTSENSOR_PIN 0
                  
DHT dht(TEMPHUMIDSENSOR_PIN, DHT11);

#define SW_PIN 19
#define SW_PIN_1 5  
#define SW_PIN_2 15  

#define RESET_PIN   13  //用于删除WiFi信息

#define LEDY 12
#define LEDG 14
boolean LEDOn_1 = false;
boolean LEDOn_2 = false;

//==============================================================

 int beforeState =1;
int nowState =1;
int debounce(int SWITCHPIN,int delaytime){

  int ispressed =0;
  nowState =digitalRead(SWITCHPIN);
  if(beforeState ==1 and nowState ==0)
  {
    ispressed =1;
    Serial.println("pressed!" );
  } 
if(nowState!=beforeState)
{
  delay(delaytime);
}
beforeState =nowState;
return ispressed;
delay(1);
}

//==============================================================
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

//温度湿度显示//
const unsigned char tC[] PROGMEM=    { 0x04,0x00,0x00,0x1e,0x00,0x00,0x1b,0x38,0x00,0x1b,0xfe,0x00,0x0e,0xff,0x03,0x80,0x83,0x03,0xc0,0x01,0x07,0xc0,0x00,0x02,0xc0,0x00,0x00,0xc0,0x00,0x00,0xe0,0x00,0x00,0xe0,0x00,0x00,0xc0,0x00,0x06,0xc0,0x00,0x07,0xc0,0x01,0x07,0x80,0xc3,0x03,0x00,0xff,0x01,0x00,0xfe,0x00,0x00,0x00,0x00 };
//20*45
const unsigned char wenduicon[] PROGMEM = { 0xe0,0x7f,0x00,0xf0,0xff,0x00,0xf0,0xff,0x00,0xf8,0xff,0x00,0x78,0xe0,0x00,0x78,0xe0,0x00,0x78,0xe0,0x00,0x78,0xe0,0x00,0x78,0xe6,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x00,0x78,0xef,0x01,0x78,0xef,0x01,0x3c,0xef,0x03,0x9e,0xcf,0x07,0xde,0x9f,0x07,0xce,0x3f,0x07,0xef,0x3f,0x07,0xef,0x3f,0x0f,0xcf,0x3f,0x07,0xcf,0x3f,0x07,0x9e,0x9f,0x07,0x1e,0x8f,0x07,0x3c,0xe0,0x03,0xfc,0xf9,0x01,0xf8,0xff,0x01,0xf0,0x7f,0x00,0xc0,0x1f,0x00 };
const unsigned char shiduicon[] PROGMEM =   { 0x00,0x00,0x00,0x30,0x00,0x00,0x78,0x00,0x00,0x78,0x00,0x00,0xfc,0x00,0x00,0xcc,0x00,0x00,0xcc,0x00,0x00,0x86,0x01,0x00,0x86,0xc1,0x01,0x86,0xc1,0x01,0x86,0xc1,0x03,0xfe,0xe1,0x03,0xfc,0x60,0x03,0x30,0x70,0x06,0x00,0x30,0x06,0x00,0x38,0x0e,0x00,0x38,0x0c,0x00,0x38,0x0e,0x00,0x30,0x06,0x00,0xf0,0x07,0x00,0xe0,0x03,0x80,0x03,0x00,0x80,0x03,0x00,0xc0,0x07,0x00,0xc0,0x07,0x00,0xe0,0x0e,0x00,0x60,0x0c,0x00,0x70,0x1c,0x00,0x30,0x18,0x00,0x38,0x30,0x00,0x18,0x30,0x00,0x1c,0x60,0x00,0x0c,0x60,0x00,0x6e,0xc0,0x00,0x66,0xc0,0x00,0x66,0xc0,0x00,0x66,0xc0,0x00,0xee,0xe0,0x00,0xcc,0x61,0x00,0x1c,0x70,0x00,0x78,0x3c,0x00,0xf0,0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
const unsigned char tH[] U8X8_PROGMEM = { 0x00,0x00,0x00,0x38,0x00,0x02,0x66,0x00,0x03,0xc6,0x80,0x01,0xc2,0xc0,0x00,0xc2,0x60,0x00,0xc2,0x30,0x00,0x46,0x18,0x00,0x7c,0x0c,0x00,0x00,0x06,0x00,0x00,0xe3,0x01,0x80,0x31,0x02,0xc0,0x10,0x06,0x60,0x10,0x06,0x30,0x10,0x06,0x1c,0x30,0x02,0x0e,0x70,0x03,0x06,0xe0,0x01,0x00,0x00,0x00 };
boolean SW_1 =false;




static const char ntpServerName[] = "ntp.nict.jp"; //NTP服务器，阿里云
int timeZone = 9;                                //时区


const char* AP_SSID  = "ESP32_Config"; //热点名称
String wifi_ssid = "yourSSID";
String wifi_pass = "yourPW";
String scanNetworksID = "";//用于储存扫描到的WiFi
//=============================================知心天气私钥===========================
const char* HOST = "http://api.seniverse.com";
const char* APIKEY = "SOaQKgbKC8wqD6wz0";        //API KEY 知心天气私钥
const char* CITY = "NANTONG";  //城市全拼
const char* LANGUAGE = "zh-Hans";//zh-Hans 简体中文

//=============================================天气参数===========================
typedef struct
{
    String date_m;
    String date_d;
    int code_day;
    int high;
    int low;
    int wind_direction_degree;
    int wind_scale;
    int humidity;
}weather_date; //天气信息的结构体

weather_date day0,day1,day2;
//==================================================================================


//==================================================================================
#define ROOT_HTML  "<!DOCTYPE html><html><head><title>WIFI Config </title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}</style><body><form method=\"GET\" action=\"connect\"><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\"></label><label class=\"input\"><span>WiFi PASS</span><input type=\"text\"  name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submie\"> <p><span> Nearby wifi:</P></form>"
//-----------------------定义常量-------------------------
const unsigned long HTTP_TIMEOUT = 5000;  //http访问请求
WebServer server(80);
WiFiClient client;
HTTPClient http;
const int slaveSelect = 5;
const int scanLimit = 7;
String response_ws;  //json返回天气信息
String GetUrl;  //心知天气url请求
//-----------------------定义常量-------------------------


WiFiUDP Udp;
unsigned int localPort = 8888; // 用于侦听UDP数据包的本地端口
time_t getNtpTime();
void sendNTPpacket(IPAddress& address);
void oledClockDisplay();
void sendCommand(int command, int value);
void initdisplay();

boolean isNTPConnected = false;

const unsigned char xing[] U8X8_PROGMEM = {
  0x00, 0x00, 0xF8, 0x0F, 0x08, 0x08, 0xF8, 0x0F, 0x08, 0x08, 0xF8, 0x0F, 0x80, 0x00, 0x88, 0x00,
  0xF8, 0x1F, 0x84, 0x00, 0x82, 0x00, 0xF8, 0x0F, 0x80, 0x00, 0x80, 0x00, 0xFE, 0x3F, 0x00, 0x00
};  /*星*/
const unsigned char liu[] U8X8_PROGMEM = { 
  0x40, 0x00, 0x80, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
  0x20, 0x02, 0x20, 0x04, 0x10, 0x08, 0x10, 0x10, 0x08, 0x10, 0x04, 0x20, 0x02, 0x20, 0x00, 0x00
};  /*六*/

//------------------------------------------------------------------
//下面是天气图标和对应心知天气的天气代码
//Sunny = 0 晴
const unsigned char Sunny[] U8X8_PROGMEM ={
0x00,0xC0,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0xC0,0x00,0x00,0x00,0xC0,0x00,0x00,
0x30,0xC0,0x00,0x03,0x70,0x00,0x80,0x03,0xE0,0x00,0xC0,0x01,0xC0,0xE0,0xC1,0x00,
0x00,0xF8,0x07,0x00,0x00,0xFE,0x1F,0x00,0x00,0xFE,0x1F,0x00,0x00,0xFF,0x3F,0x00,
0x00,0xFF,0x3F,0x00,0x80,0xFF,0x7F,0x00,0x8F,0xFF,0x7F,0x3C,0x8F,0xFF,0x7F,0x3C,
0x80,0xFF,0x7F,0x00,0x80,0xFF,0x7F,0x00,0x00,0xFF,0x3F,0x00,0x00,0xFE,0x1F,0x00,
0x00,0xFE,0x1F,0x00,0x00,0xFC,0x0F,0x00,0xC0,0xF0,0xC3,0x00,0xE0,0x00,0xC0,0x01,
0x70,0x00,0x80,0x03,0x30,0x00,0x00,0x03,0x00,0xC0,0x00,0x00,0x00,0xC0,0x00,0x00,
0x00,0xC0,0x00,0x00,0x00,0xC0,0x00,0x00                                            
};
//Cloudy = 4 多云
const unsigned char Cloudy[] U8X8_PROGMEM ={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,
0x00,0x80,0x3F,0x00,0x00,0xE6,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x80,0xFF,0xFF,0x00,
0x80,0xFF,0xFF,0x01,0xC0,0xFF,0xFF,0x01,0xC0,0xFF,0xFF,0x03,0xE0,0xFF,0xFF,0x07,
0xF0,0xFF,0xFF,0x0F,0xF8,0xFF,0x07,0x0F,0xF8,0xFF,0x71,0x0F,0xF8,0xFF,0xFE,0x0E,
0xF8,0x7F,0xFE,0x08,0xF0,0x7F,0xFF,0x07,0xE0,0x1F,0xFF,0x0F,0xC0,0xEF,0xFF,0x1F,
0x80,0xE7,0xFF,0x1F,0x00,0xF0,0xFF,0x3F,0x1C,0xF0,0xFF,0x3F,0x3C,0xF0,0xFF,0x1F,
0x7E,0xF0,0xFF,0x1F,0x7F,0xE0,0xFF,0x0F,0x3E,0xC0,0xFF,0x07,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                                            
};
//Overcast = 9 阴
const unsigned char Overcast[] U8X8_PROGMEM ={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x80,0x1F,0x00,0x00,0xE0,0x7F,0x00,0x00,0xF3,0xFF,0x00,
0xC0,0xFF,0xFF,0x01,0xE0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x03,
0xF0,0xFF,0xFF,0x03,0xF8,0xFF,0xFF,0x07,0xFC,0xFF,0xFF,0x0F,0xFE,0xFF,0xFF,0x1F,
0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,
0xFF,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x0F,0xF8,0xFF,0xFF,0x07,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00                                      
};
//Shower = 10 阵雨
const unsigned char Shower[] U8X8_PROGMEM ={
0x00,0x00,0x70,0x00,0x00,0x00,0xFC,0x03,0x00,0x00,0xFE,0x07,0x00,0x00,0xF8,0x0F,
0x00,0xC0,0xE3,0x1F,0x00,0xF0,0xCF,0x1F,0xC0,0xF9,0x9F,0x3F,0xE0,0xFF,0xBF,0x3F,
0xF0,0xFF,0x3F,0x3F,0xF0,0xFF,0x7F,0x1F,0xF0,0xFF,0x7F,0x1E,0xF8,0xFF,0xFF,0x1C,
0xFC,0xFF,0xFF,0x09,0xFE,0xFF,0xFF,0x03,0xFE,0xFF,0xFF,0x03,0xFF,0xFF,0xFF,0x07,
0xFF,0xFF,0xFF,0x07,0xFF,0xFF,0xFF,0x07,0xFF,0xFF,0xFF,0x03,0xFE,0xFF,0xFF,0x03,
0xFC,0xFF,0xFF,0x01,0xF0,0xFF,0x7F,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x20,0x00,
0x60,0x60,0x30,0x00,0x60,0x60,0x30,0x00,0x30,0x30,0x18,0x00,0x30,0x30,0x18,0x00,
0x18,0x18,0x0C,0x00,0x08,0x08,0x04,0x00                                           
};
//Thundershower = 11 雷阵雨
const unsigned char Thundershower[] U8X8_PROGMEM ={
0x00,0x00,0x0E,0x00,0x00,0xC0,0x3F,0x00,0x00,0xE0,0xFF,0x00,0xC0,0xFF,0xFF,0x01,
0xE0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x03,0xF0,0xFF,0xFF,0x03,
0xF0,0xFF,0xFF,0x03,0xF8,0xFF,0xFF,0x07,0xFC,0xFF,0xFF,0x0F,0xFE,0xFF,0xFF,0x1F,
0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,
0xFE,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x0F,0xFC,0xFF,0xFF,0x07,0xF0,0xFF,0xFF,0x01,
0x00,0xF0,0x01,0x00,0x60,0xF0,0xC1,0x00,0x60,0xF0,0xC0,0x00,0x60,0x78,0xC0,0x00,
0x30,0xFC,0x60,0x00,0x30,0x70,0x60,0x00,0x10,0x30,0x20,0x00,0x10,0x18,0x20,0x00,
0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x00                                           
};
//Rain_L = 13 小雨
const unsigned char Rain_L[] U8X8_PROGMEM ={
0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0xC0,0x3F,0x00,0x00,0xE0,0xFF,0x00,
0xC0,0xFF,0xFF,0x01,0xE0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x03,
0xF0,0xFF,0xFF,0x03,0xF0,0xFF,0xFF,0x03,0xF8,0xFF,0xFF,0x0F,0xFC,0xFF,0xFF,0x0F,
0xFE,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,
0xFF,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x0F,0xFC,0xFF,0xFF,0x07,
0xE0,0xFF,0xFF,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x60,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x10,0x00,0x00,
0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00                                            
};
//Rain_M = 14 中雨
const unsigned char Rain_M[] U8X8_PROGMEM ={
0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0xC0,0x3F,0x00,0x00,0xE0,0xFF,0x00,
0xC0,0xFF,0xFF,0x01,0xE0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x03,
0xF0,0xFF,0xFF,0x03,0xF0,0xFF,0xFF,0x03,0xF8,0xFF,0xFF,0x0F,0xFC,0xFF,0xFF,0x0F,
0xFE,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,
0xFF,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x0F,0xFC,0xFF,0xFF,0x07,
0xE0,0xFF,0xFF,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x0C,0x0C,0x00,0x00,0x04,0x04,0x00,0x00,0x06,0x06,0x00,0x00,0x02,0x02,0x00,
0x00,0x03,0x03,0x00,0x00,0x00,0x00,0x00                                   
};
//Rain_H = 15 大雨
const unsigned char Rain_H[] U8X8_PROGMEM ={
0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0xC0,0x3F,0x00,0x00,0xE0,0xFF,0x00,
0xC0,0xFF,0xFF,0x01,0xE0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x03,
0xF0,0xFF,0xFF,0x03,0xF0,0xFF,0xFF,0x03,0xF8,0xFF,0xFF,0x0F,0xFC,0xFF,0xFF,0x0F,
0xFE,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,0xFF,0xFF,0xFF,0x1F,
0xFF,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x0F,0xFC,0xFF,0xFF,0x07,
0xE0,0xFF,0xFF,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x60,0x60,0x60,0x00,0x20,0x20,0x20,0x00,0x30,0x30,0x30,0x00,0x10,0x10,0x10,0x00,
0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00                                            
};
//Foggy = 30 雾
const unsigned char Foggy[] U8X8_PROGMEM ={
0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0xC0,0xFF,0x00,0x00,0xF0,0xFF,0x01,0x00,
0xF8,0xFF,0x03,0x00,0xFC,0xFF,0x07,0x00,0xFC,0xFF,0x0F,0x00,0xFE,0xFF,0xFF,0x01,
0xFE,0xFF,0xFF,0x07,0xFE,0xFF,0xFF,0x0F,0xFE,0xFF,0xFF,0x0F,0xFE,0xFF,0xFF,0x1F,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFF,0xFF,0x1F,
0xFE,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xFE,0xFF,0xFF,0x1F,0xFE,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0xFF,0xFF,0x1F,
0xFE,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00                                            
};
//Haze = 31 霾
const unsigned char Haze[] U8X8_PROGMEM ={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0xE0,0x00,0x1C,0x0E,0xE0,0x00,0x1C,
0x0E,0xE0,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xF0,0x03,0xF0,0x01,0xF8,0x0F,0xFC,0x07,0x1C,0x1E,0x0E,0x0E,
0x0E,0x38,0x07,0x1C,0x06,0x30,0x03,0x18,0x07,0xF0,0x03,0x38,0x03,0xE0,0x01,0x30,
0x03,0xE0,0x01,0x30,0x07,0x30,0x03,0x38,0x06,0x38,0x07,0x18,0x0E,0x1C,0x0E,0x1C,
0x1C,0x0F,0x1C,0x0F,0xF8,0x07,0xF8,0x03,0xF0,0x03,0xF0,0x01,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0xE0,0x00,0x1C,0x0E,0xE0,0x00,0x1C,
0x0E,0xE0,0x00,0x1C,0x00,0x00,0x00,0x00                                            
};

//=---------------------------------------------------------------------



//********************wifi配置与ntp服务器连接*************************************//
//用于配置WiFi
void wifi_Config()
{
  Serial.println("scan start");
  // 扫描附近WiFi
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
    scanNetworksID = "no networks found";
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      scanNetworksID += "<P>" + WiFi.SSID(i) + "</P>";
      delay(10);
    }
  }
  Serial.println("");

  WiFi.mode(WIFI_AP);//配置为AP模式
  boolean result = WiFi.softAP(AP_SSID, ""); //开启WIFI热点
  if (result)
  {
    delay(2000);
    IPAddress myIP = WiFi.softAPIP();
    delay(2000); 
    //打印相关信息
    Serial.println("");
    Serial.print("Soft-AP IP address = ");
    Serial.println(myIP);
    Serial.println(String("MAC address = ")  + WiFi.softAPmacAddress().c_str());
    Serial.println("waiting ...");

  u8g2.clearBuffer();
  u8g2.setCursor(0, 14);
  u8g2.print("Plz reset WiFi&PW...");
  u8g2.setCursor(0, 47);
  u8g2.print("192.168.4.1");
  u8g2.sendBuffer();
  } else {  //开启热点失败
    Serial.println("WiFiAP Failed");
    
    delay(3000);
    ESP.restart();  //复位esp32
  }

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  //首页
  server.on("/", []() {
    server.send(200, "text/html", ROOT_HTML + scanNetworksID + "</body></html>");
  });

  //连接
  server.on("/connect", []() {

    server.send(200, "text/html", "<html><body><font size=\"10\">successd,wifi connecting...<br />Please close this page manually.</font></body></html>");

    WiFi.softAPdisconnect(true);
    //获取输入的WIFI账户和密码
    wifi_ssid = server.arg("ssid");
    wifi_pass = server.arg("pass");
    server.close();
    WiFi.softAPdisconnect();
    Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS:" + wifi_pass);

    //设置为STA模式并连接WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str());
    uint8_t Connect_time = 0; //用于连接计时，如果长时间连接不成功，复位设备
    while (WiFi.status() != WL_CONNECTED) {  //等待WIFI连接成功
      delay(500);
      Serial.print(".");
      Connect_time ++;
      if (Connect_time > 80) {  //长时间连接不上，复位设备
        Serial.println("Connection timeout, check input is correct or try again later!");
        delay(3000);
        ESP.restart();
      }
    }
   
 if(WiFi.status() == WL_CONNECTED) {
   Serial.println("");
    Serial.println("WIFI Config Success");
    Serial.printf("SSID:%s", WiFi.SSID().c_str());
    Serial.print("  LocalIP:");
    Serial.print(WiFi.localIP());
    Serial.println("");
    //WIFI已连接
  u8g2.clearBuffer();
  u8g2.setCursor(0, 14);
  u8g2.print("WiFi connected...");
  u8g2.sendBuffer();
  delay(2000);
  setSyncProvider(getNtpTime);
  setSyncInterval(30); //每300秒同步一次时间
  isNTPConnected = true;
  }

  });
  server.begin();
   Serial.println("Web server started");
}


bool AutoConfig()
{
  WiFi.begin();
  for (int i = 0; i < 10; i++)
  {
    int wstatus = WiFi.status();
    if (wstatus == WL_CONNECTED)
    {
      Serial.println("WIFI SmartConfig Success");
      Serial.printf("SSID:%s", WiFi.SSID().c_str());
      Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
      Serial.print("LocalIP:");
      Serial.print(WiFi.localIP());
      Serial.print(" ,GateIP:");
      Serial.println(WiFi.gatewayIP());
      return true;
    }
    else
    {
      Serial.print("WIFI AutoConfig Waiting......");
      Serial.println(wstatus);
      delay(1000);
    }
  }
  Serial.println("WIFI AutoConfig Faild!" );
  return false;
}
//********************wifi配置与ntp服务器连接*************************************//

//********************setup*************************************//

//********************setup*************************************//

void setup() {

Serial.begin(115200);
while (!Serial)
    continue;
    pinMode(LEDY,OUTPUT);
  pinMode(LEDG,OUTPUT);
  pinMode(SW_PIN, INPUT_PULLUP);
  pinMode(SW_PIN_1, INPUT_PULLUP);
  pinMode(SW_PIN_2, INPUT_PULLUP);
    
    //用于删除已存WiFi
   if (digitalRead(SW_PIN_2) == LOW) {
    delay(200);
    esp_wifi_restore();
    delay(10);
    ESP.restart();  //复位esp32
  } 
  
  dht.begin();
  
  initdisplay();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.setCursor(0, 14);
  u8g2.print("Waiting for WiFi...");
  u8g2.setCursor(0, 47);
  u8g2.print("ESP32ntpCLOCK v1.0");
  u8g2.setCursor(0, 64);
  u8g2.print("192.168.4.1");
  u8g2.sendBuffer();
 

  // 连接WiFi
  if (!AutoConfig())
  {
    wifi_Config();
  }
  if(WiFi.status() == WL_CONNECTED) {
    //WIFI已连接
  u8g2.clearBuffer();
  u8g2.setCursor(0, 14);
  u8g2.print("WiFi connected...");
  u8g2.sendBuffer();
  delay(2000);
  setSyncProvider(getWeather);
  setSyncInterval(360); //每1小时同步一次时间
  setSyncProvider(getNtpTime);
  setSyncInterval(30); //每300秒同步一次时间
  isNTPConnected = true;
  }
}

//===========================================================================================================
//===========================================================================================================

time_t prevDisplay = 0;   //当时钟已经显示
void savePowerMode(){
  
  if(debounce(SW_PIN_2,50)==1)
  { 
    if (SW_1==false){
      u8g2.setPowerSave(1);
      SW_1 =!SW_1;
    Serial.println("shut down the screen");
    delay(50);
    }else{
      u8g2.setPowerSave(0);
      SW_1 =!SW_1;
      delay(50);
    }
    
   
  } 
}
int i=0;
int pageChange(){
  if(debounce(SW_PIN_1,50)==1)
  {
    if(i<2 )
    {
      i=i+1;
    }else i=0;
    Serial.println(i);
  }
  return i;
  delay(1);
}

//======================================================================================
//------------------------weather_station---------------------------
//----------------------------api test------------------------------

bool getJson_ws()  //构建并发出获取天气数据的请求，获得json数据
{
    bool s = false;
    GetUrl = String(HOST) + "/v3/weather/daily.json?key=";
    GetUrl += APIKEY;
    GetUrl += "&location=";
    GetUrl += CITY;
    GetUrl += "&language=";
    GetUrl += LANGUAGE;
    GetUrl += "&unit=c&start=0&days=3";
    http.setTimeout(HTTP_TIMEOUT);
    http.begin(GetUrl);
    int httpCode = http.GET();
    if (httpCode > 0){
        if (httpCode == HTTP_CODE_OK){
            response_ws = http.getString();
            //Serial.println(response_ws);
            s = true;
        }
    }else{
        Serial.printf("[HTTP] GET JSON failed, error: %s\n", http.errorToString(httpCode).c_str());
        s = false;
    }
    http.end();
    return s;
}

bool parseJson_ws(String json)  //对心知天气的json数据解析，采用arduinojson v6库
{
    const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 3*JSON_OBJECT_SIZE(14) + 810;
    DynamicJsonDocument doc(capacity);  //根据json数据结构计算所需要的内存大小
    deserializeJson(doc, json);  //反序列化json数据
    JsonObject results_0 = doc["results"][0];
    JsonArray results_0_daily = results_0["daily"];

    JsonObject results_0_daily_0 = results_0_daily[0];  //今天的天气数据
    const char* results_0_daily_0_date = results_0_daily_0["date"]; // "2020-06-20"
    const char* results_0_daily_0_text_day = results_0_daily_0["text_day"]; // "多云"
    const char* results_0_daily_0_code_day = results_0_daily_0["code_day"]; // "4"
    const char* results_0_daily_0_text_night = results_0_daily_0["text_night"]; // "多云"
    const char* results_0_daily_0_code_night = results_0_daily_0["code_night"]; // "4"
    const char* results_0_daily_0_high = results_0_daily_0["high"]; // "28"
    const char* results_0_daily_0_low = results_0_daily_0["low"]; // "20"
    const char* results_0_daily_0_rainfall = results_0_daily_0["rainfall"]; // "0.0"
    const char* results_0_daily_0_precip = results_0_daily_0["precip"]; // ""
    const char* results_0_daily_0_wind_direction = results_0_daily_0["wind_direction"]; // "东"
    const char* results_0_daily_0_wind_direction_degree = results_0_daily_0["wind_direction_degree"]; // "90"
    const char* results_0_daily_0_wind_speed = results_0_daily_0["wind_speed"]; // "16.20"
    const char* results_0_daily_0_wind_scale = results_0_daily_0["wind_scale"]; // "3"
    const char* results_0_daily_0_humidity = results_0_daily_0["humidity"]; // "72"

    JsonObject results_0_daily_1 = results_0_daily[1];  //明天的天气数据
    const char* results_0_daily_1_date = results_0_daily_1["date"]; // "2020-06-21"
    const char* results_0_daily_1_text_day = results_0_daily_1["text_day"]; // "多云"
    const char* results_0_daily_1_code_day = results_0_daily_1["code_day"]; // "4"
    const char* results_0_daily_1_text_night = results_0_daily_1["text_night"]; // "阴"
    const char* results_0_daily_1_code_night = results_0_daily_1["code_night"]; // "9"
    const char* results_0_daily_1_high = results_0_daily_1["high"]; // "27"
    const char* results_0_daily_1_low = results_0_daily_1["low"]; // "20"
    const char* results_0_daily_1_rainfall = results_0_daily_1["rainfall"]; // "0.0"
    const char* results_0_daily_1_precip = results_0_daily_1["precip"]; // ""
    const char* results_0_daily_1_wind_direction = results_0_daily_1["wind_direction"]; // "东南"
    const char* results_0_daily_1_wind_direction_degree = results_0_daily_1["wind_direction_degree"]; // "135"
    const char* results_0_daily_1_wind_speed = results_0_daily_1["wind_speed"]; // "16.20"
    const char* results_0_daily_1_wind_scale = results_0_daily_1["wind_scale"]; // "3"
    const char* results_0_daily_1_humidity = results_0_daily_1["humidity"]; // "68"

    JsonObject results_0_daily_2 = results_0_daily[2];  //后天的天气数据
    const char* results_0_daily_2_date = results_0_daily_2["date"]; // "2020-06-22"
    const char* results_0_daily_2_text_day = results_0_daily_2["text_day"]; // "中雨"
    const char* results_0_daily_2_code_day = results_0_daily_2["code_day"]; // "14"
    const char* results_0_daily_2_text_night = results_0_daily_2["text_night"]; // "小雨"
    const char* results_0_daily_2_code_night = results_0_daily_2["code_night"]; // "13"
    const char* results_0_daily_2_high = results_0_daily_2["high"]; // "26"
    const char* results_0_daily_2_low = results_0_daily_2["low"]; // "23"
    const char* results_0_daily_2_rainfall = results_0_daily_2["rainfall"]; // "10.0"
    const char* results_0_daily_2_precip = results_0_daily_2["precip"]; // ""
    const char* results_0_daily_2_wind_direction = results_0_daily_2["wind_direction"]; // "东南"
    const char* results_0_daily_2_wind_direction_degree = results_0_daily_2["wind_direction_degree"]; // "127"
    const char* results_0_daily_2_wind_speed = results_0_daily_2["wind_speed"]; // "25.20"
    const char* results_0_daily_2_wind_scale = results_0_daily_2["wind_scale"]; // "4"
    const char* results_0_daily_2_humidity = results_0_daily_2["humidity"]; // "71"
    
    const char* results_0_last_update = results_0["last_update"];  //数据更新时间
    
    String date0 = results_0_daily_0_date;  //将日期取出处理
    String date1 = results_0_daily_1_date;  
    String date2 = results_0_daily_2_date; 
    
    day0.date_m = date0.substring(5, 7);  //日期字符串切片
    day0.date_d = date0.substring(8, 10);
    day0.code_day = atoi(results_0_daily_0_code_day);//获取今天天气信息
    day0.high = atoi(results_0_daily_0_high);  //最高温度
    day0.low = atoi(results_0_daily_0_low);  //最低温度
    //day0.wind_direction_degree = atoi(results_0_daily_0_wind_direction_degree);  //风向
    //day0.wind_scale = atoi(results_0_daily_0_wind_scale);  //风力等级
    //day0.humidity = atoi(results_0_daily_0_humidity);  //湿度
    
    day1.date_m = date1.substring(5, 7);
    day1.date_d = date1.substring(8, 10);
    day1.code_day = atoi(results_0_daily_1_code_day);//获取明天天气信息
    day1.high = atoi(results_0_daily_1_high);
    day1.low = atoi(results_0_daily_1_low);
    //day1.wind_direction_degree = atoi(results_0_daily_1_wind_direction_degree);
    //day1.wind_scale = atoi(results_0_daily_1_wind_scale);
    //day1.humidity = atoi(results_0_daily_1_humidity);

    day2.date_m = date2.substring(5, 7);
    day2.date_d = date2.substring(8, 10);
    day2.code_day = atoi(results_0_daily_2_code_day);//获取后天天气信息
    day2.high = atoi(results_0_daily_2_high);
    day2.low = atoi(results_0_daily_2_low);
    //day2.wind_direction_degree = atoi(results_0_daily_2_wind_direction_degree);
    //day2.wind_scale = atoi(results_0_daily_2_wind_scale);
    //day2.humidity = atoi(results_0_daily_2_humidity);

    return true;
}

//========================================================================================================
void display_today()  //oled显示今天的天气信息
{   

u8g2.clearBuffer();
u8g2.setFont(u8g2_font_unifont_t_japanese2);  
u8g2.setFontDirection(0);

u8g2.setCursor(0, 14);
u8g2.print("本日天気");
//u8g2.print(day0.date_m);
//u8g2.print("月");
//u8g2.setCursor(31, 14);
//u8g2.print(day0.date_d);
//u8g2.print("日");
u8g2.setCursor(68, 14);
u8g2.print("NANTONG");

u8g2.setFont(u8g2_font_tenstamps_mu);
  u8g2.setCursor(39,30);
  u8g2.print("MIN");
   u8g2.setCursor(83,60);
  u8g2.print("MAX");
  
  u8g2.setFont(u8g2_font_freedoomr25_tn);
  u8g2.setCursor(38,64);
  //u8g2.print("0");
  u8g2.print(day0.low);
   u8g2.setCursor(88,45);
  u8g2.print(day0.high);
 u8g2.setFont(u8g2_font_unifont_t_japanese2); 
switch(day0.code_day)  //将天气代码转化为天气图标和字符显示
    {
        case 0:
            u8g2.drawXBMP(5 , 17 , 30 , 30 ,Sunny );
            u8g2.setCursor(6,63);
            u8g2.print("晴れ");
            break;
        case 4:
            u8g2.drawXBMP(5 , 17, 30 , 30 , Cloudy );
            u8g2.setCursor(6,63);
            u8g2.print("曇り");
            break;
        case 9:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Overcast );
            u8g2.setCursor(6,63);
            u8g2.print("曇り");
            break;
        case 10:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Shower );
            u8g2.setCursor(9,63);
            u8g2.print("雨");
            break;
        case 11:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Thundershower );
            u8g2.setCursor(6,63);
            u8g2.print("雷雨");
            break;
        case 13:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_L );
            u8g2.setCursor(6,63);
            u8g2.print("小雨");
            break;
        case 14:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_M );
            u8g2.setCursor(6,63);
            u8g2.print("中雨");
            break;
        case 15:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_H );
            u8g2.setCursor(6,63);
            u8g2.print("大雨");
            break;
        case 30:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Foggy );
            u8g2.setCursor(9,63);
            u8g2.print("霧");
            break;
        case 31:
            u8g2.drawXBMP( 5 , 17, 30 , 30 , Haze );
            u8g2.setCursor(6,63);
            u8g2.print("ばい");
           
            break;    
    }
    
    u8g2.sendBuffer();
    
}
//==========================================================================================
//========================================================================================================
void display_day_1()  //oled显示明天的天气信息
{   
  u8g2.clearBuffer();
u8g2.setFont(u8g2_font_unifont_t_japanese2);  
u8g2.setFontDirection(0);

u8g2.setCursor(0, 14);
u8g2.print("明日天気");
//u8g2.print(day1.date_m);
//u8g2.print("月");
//u8g2.setCursor(31, 14);
//u8g2.print(day1.date_d);
//u8g2.print("日");
u8g2.setCursor(68, 14);
u8g2.print("NANTONG");

u8g2.setFont(u8g2_font_tenstamps_mu);
  u8g2.setCursor(39,30);
  u8g2.print("MIN");
   u8g2.setCursor(83,60);
  u8g2.print("MAX");
  
  u8g2.setFont(u8g2_font_freedoomr25_tn);
  u8g2.setCursor(38,64);
  //u8g2.print("0");
  u8g2.print(day1.low);
   u8g2.setCursor(88,45);
  u8g2.print(day1.high);
 u8g2.setFont(u8g2_font_unifont_t_japanese2); 
switch(day1.code_day)  //将天气代码转化为天气图标和字符显示
    {
        case 0:
            u8g2.drawXBMP(5 , 17 , 30 , 30 ,Sunny );
            u8g2.setCursor(6,63);
            u8g2.print("晴れ");
            break;
        case 4:
            u8g2.drawXBMP(5 , 17, 30 , 30 , Cloudy );
            u8g2.setCursor(6,63);
            u8g2.print("曇り");
            break;
        case 9:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Overcast );
            u8g2.setCursor(6,63);
            u8g2.print("曇り");
            break;
        case 10:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Shower );
            u8g2.setCursor(9,63);
            u8g2.print("雨");
            break;
        case 11:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Thundershower );
            u8g2.setCursor(6,63);
            u8g2.print("雷雨");
            break;
        case 13:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_L );
            u8g2.setCursor(6,63);
            u8g2.print("小雨");
            break;
        case 14:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_M );
            u8g2.setCursor(6,63);
            u8g2.print("中雨");
            break;
        case 15:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_H );
            u8g2.setCursor(6,63);
            u8g2.print("大雨");
            break;
        case 30:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Foggy );
            u8g2.setCursor(9,63);
            u8g2.print("霧");
            break;
        case 31:
            u8g2.drawXBMP( 5 , 17, 30 , 30 , Haze );
            u8g2.setCursor(6,63);
            u8g2.print("ばい");
           
            break;    
    }
    
    u8g2.sendBuffer();
  
}
//========================================================================
//========================================================================================================
void display_day_2()  //oled显示后天的天气信息
{   
  u8g2.clearBuffer();
u8g2.setFont(u8g2_font_unifont_t_japanese2);  
u8g2.setFontDirection(0);

u8g2.setCursor(0, 14);
u8g2.print("明後日");
//u8g2.print(day2.date_m);
//u8g2.print("月");
//u8g2.setCursor(31, 14);
//u8g2.print(day2.date_d);
//u8g2.print("日");
u8g2.setCursor(68, 14);
u8g2.print("NANTONG");

u8g2.setFont(u8g2_font_tenstamps_mu);
  u8g2.setCursor(39,30);
  u8g2.print("MIN");
   u8g2.setCursor(83,60);
  u8g2.print("MAX");
  
  u8g2.setFont(u8g2_font_freedoomr25_tn);
  u8g2.setCursor(38,64);
  //u8g2.print("0");
  u8g2.print(day2.low);
   u8g2.setCursor(88,45);
  u8g2.print(day2.high);
 u8g2.setFont(u8g2_font_unifont_t_japanese2); 
switch(day2.code_day)  //将天气代码转化为天气图标和字符显示
    {
        case 0:
            u8g2.drawXBMP(5 , 17 , 30 , 30 ,Sunny );
            u8g2.setCursor(6,63);
            u8g2.print("晴れ");
            break;
        case 4:
            u8g2.drawXBMP(5 , 17, 30 , 30 , Cloudy );
            u8g2.setCursor(6,63);
            u8g2.print("曇り");
            break;
        case 9:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Overcast );
            u8g2.setCursor(6,63);
            u8g2.print("曇り");
            break;
        case 10:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Shower );
            u8g2.setCursor(9,63);
            u8g2.print("雨");
            break;
        case 11:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Thundershower );
            u8g2.setCursor(6,63);
            u8g2.print("雷雨");
            break;
        case 13:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_L );
            u8g2.setCursor(6,63);
            u8g2.print("小雨");
            break;
        case 14:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_M );
            u8g2.setCursor(6,63);
            u8g2.print("中雨");
            break;
        case 15:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Rain_H );
            u8g2.setCursor(6,63);
            u8g2.print("大雨");
            break;
        case 30:
            u8g2.drawXBMP( 5 , 17 , 30 , 30 , Foggy );
            u8g2.setCursor(9,63);
            u8g2.print("霧");
            break;
        case 31:
            u8g2.drawXBMP( 5 , 17, 30 , 30 , Haze );
            u8g2.setCursor(6,63);
            u8g2.print("ばい");
           
            break;    
    }
    
    u8g2.sendBuffer();
  
}
//========================================================================
boolean isgetWeather =false;
time_t getWeather(){
  isgetWeather =false;
  if (getJson_ws()){
        if(parseJson_ws(response_ws))
        {
          isgetWeather =true;
        }
}
}
//==========================================================================
int j=0;
int weather_p(){
  
   if(debounce(SW_PIN,50)==1)
            {
              if(j<2 )
              {
                j++;
               }else j=0;
                
              }
                return j;
                delay(50);
             }
                         
void showweather(){

  
        if(isgetWeather ==true){
          
          int wp=weather_p();
         
           switch(wp)
           {
            case 0:
            display_today();
            delay(50);
            break;
            case 1:
            display_day_1();
            delay(50);
            break;
            case 2:
            display_day_2();
            delay(50);
            break;
           }
        }else{
         u8g2.clearBuffer();
          u8g2.setFont(u8g2_font_unifont_t_japanese2);  
          u8g2.setFontDirection(0);
          u8g2.setCursor(0, 14);
          u8g2.print("Searching...");
          u8g2.sendBuffer();
        }
}
//==================================================================================

//==================================================================================
void loop() {
  server.handleClient();
  
  
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int humidity_1 =round(humidity);
  int temperature_1=round(temperature);
  //Serial.println(humidity_1);
  //Serial.println(temperature_1);
  int p =pageChange();
  //savePower();
  //Serial.println(digitalRead(SW_PIN_1));
  //Serial.println(p);
 
 
   switch(p){
    case 0:
    savePowerMode();
    timeshow();
    delay(50);
    break;
    case 1:
    savePowerMode();
    showFont(humidity_1,temperature_1);
    delay(50);
    break;
    case 2:
    savePowerMode();
    showweather();
    delay(50);
    break;
  }
//savePower();
  delay(5);
}

//********************温度 湿度显示代码*************************************//
void showFont(int h,int t){
  u8g2.enableUTF8Print();   
  u8g2.setFontDirection(0);
 
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_logisoso28_tn);
    u8g2.setCursor(22,45);
    u8g2.print(t);
    u8g2.setCursor(92,45);
    u8g2.print(h);
    u8g2.drawXBM(70,16,20,44,shiduicon);
    u8g2.drawXBM(0,16,20,45,wenduicon);
    u8g2.drawXBM(36,46,20,20,tC);
    u8g2.drawXBM(110,46,19,19,tH);
    u8g2.setFont(u8g2_font_unifont_t_japanese2);
    u8g2.setCursor(25, 14);
    u8g2.print("温度");
    u8g2.setCursor(95, 14);
    u8g2.print("湿度");
  } while ( u8g2.nextPage() );
  delay(30);
}






//********************时间显示代码*************************************//
void timeshow(){
  if (timeStatus() != timeNotSet)
  {

    if (now() != prevDisplay)
    { //时间改变时更新显示
      int tp=time_p();
      switch (tp)
      {
        case 0:
     
         prevDisplay = now();
         oledClockDisplay();
      
         break;

         case 1:
    
          prevDisplay = now();
          oledClockDisplay_1();

         break;
      }
     
    }
  }
}
void initdisplay()
{
  u8g2.begin();
  u8g2.enableUTF8Print();
}

void oledClockDisplay()
{
  int years, months, days, hours, minutes, seconds, weekdays;
  years = year();
  months = month();
  days = day();
  hours = hour();
  minutes = minute();
  seconds = second();
  weekdays = weekday();
  Serial.printf("%d/%d/%d %d:%d:%d Weekday:%d\n", years, months, days, hours, minutes, seconds, weekdays);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_japanese2);
  u8g2.setCursor(0, 14);
  if (isNTPConnected)
    u8g2.print("今の時間 (UTC+9)");
  else
    u8g2.print("No WIFI"); //如果上次对时失败，则会显示无网络
  String currentTime = "";
  if (hours < 10)
    currentTime += 0;
  currentTime += hours;
  currentTime += ":";
  if (minutes < 10)
    currentTime += 0;
  currentTime += minutes;
  currentTime += ":";
  if (seconds < 10)
    currentTime += 0;
  currentTime += seconds;
  String currentDay = "";
  currentDay += years;
  currentDay += "/";
  if (months < 10)
    currentDay += 0;
  currentDay += months;
  currentDay += "/";
  if (days < 10)
    currentDay += 0;
  currentDay += days;

  u8g2.setFont(u8g2_font_logisoso24_tr);
  u8g2.setCursor(0, 44);
  u8g2.print(currentTime);
  u8g2.setCursor(0, 61);
  u8g2.setFont(u8g2_font_unifont_t_japanese2);
  u8g2.print(currentDay);
  u8g2.setCursor(98,62);
  u8g2.print("曜");
  u8g2.setCursor(112, 62);
  u8g2.print("日");
  u8g2.setCursor(82, 62);
  if (weekdays == 1)
    u8g2.print("日");
  else if (weekdays == 2)
    u8g2.print("月");
  else if (weekdays == 3)
    u8g2.print("火");
  else if (weekdays == 4)
    u8g2.print("水");
  else if (weekdays == 5)
    u8g2.print("木");
  else if (weekdays == 6)
    u8g2.print("金");
  else if (weekdays == 7)
    u8g2.print("土");
    //u8g2.drawXBM(111, 49, 16, 16, liu);
  u8g2.sendBuffer();
}

//=================================================================================中国时间=====================================
int y=0;
int time_p(){
  
   if(debounce(SW_PIN,50)==1)
            {
              if(j<1 )
              {
                j++;
               }else j=0;
                
              }
                return j;
                delay(20);
             }
void oledClockDisplay_1()
{
   int years, months, days, hours, minutes, seconds, weekdays;
  years = year();
  months = month();
  days = day();
  hours = hour()-1;
  minutes = minute();
  seconds = second();
  weekdays = weekday();
  
  Serial.printf("%d/%d/%d %d:%d:%d Weekday:%d\n", years, months, days, hours, minutes, seconds, weekdays);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.setCursor(0, 14);
  if (isNTPConnected)
    u8g2.print("当前时间 (UTC+8)");
  else
    u8g2.print("无网络!"); //如果上次对时失败，则会显示无网络
  String currentTime = "";
  if (hours < 10)
    currentTime += 0;
  currentTime += hours;
  currentTime += ":";
  if (minutes < 10)
    currentTime += 0;
  currentTime += minutes;
  currentTime += ":";
  if (seconds < 10)
    currentTime += 0;
  currentTime += seconds;
  String currentDay = "";
  currentDay += years;
  currentDay += "/";
  if (months < 10)
    currentDay += 0;
  currentDay += months;
  currentDay += "/";
  if (days < 10)
    currentDay += 0;
  currentDay += days;

  u8g2.setFont(u8g2_font_logisoso24_tr);
  u8g2.setCursor(0, 44);
  u8g2.print(currentTime);
  u8g2.setCursor(0, 61);
  u8g2.setFont(u8g2_font_unifont_t_chinese2);
  u8g2.print(currentDay);
  u8g2.drawXBM(80, 48, 16, 16, xing);
  u8g2.setCursor(95, 62);
  u8g2.print("期");
  if (weekdays == 1)
    u8g2.print("日");
  else if (weekdays == 2)
    u8g2.print("一");
  else if (weekdays == 3)
    u8g2.print("二");
  else if (weekdays == 4)
    u8g2.print("三");
  else if (weekdays == 5)
    u8g2.print("四");
  else if (weekdays == 6)
    u8g2.print("五");
  else if (weekdays == 7)
    u8g2.drawXBM(111, 49, 16, 16, liu);
  u8g2.sendBuffer();
}
/*-------- NTP 代码 ----------*/

const int NTP_PACKET_SIZE = 48;     // NTP时间在消息的前48个字节里
byte packetBuffer[NTP_PACKET_SIZE]; // 输入输出包的缓冲区

time_t getNtpTime()
{
  IPAddress ntpServerIP;          // NTP服务器的地址

  while (Udp.parsePacket() > 0);  // 丢弃以前接收的任何数据包
  Serial.println("Transmit NTP Request");
  // 从池中获取随机服务器
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500)
  {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE)
    {
      Serial.println("Receive NTP Response");
      isNTPConnected = true;
      Udp.read(packetBuffer, NTP_PACKET_SIZE); // 将数据包读取到缓冲区
      unsigned long secsSince1900;
      // 将从位置40开始的四个字节转换为长整型，只取前32位整数部分
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      Serial.println(secsSince1900);
      Serial.println(secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-("); //无NTP响应
  isNTPConnected = false;
  return 0; //如果未得到时间则返回0
}

// 向给定地址的时间服务器发送NTP请求
void sendNTPpacket(IPAddress& address)
{
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  Udp.beginPacket(address, 123); //NTP需要使用的UDP端口号为123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();

}
//********************时间显示代码*************************************//

/*void savePower()
{

  int darkness = analogRead(LIGHTSENSOR_PIN);
   Serial.println(darkness);
  if(darkness > 2000)
  {
    u8g2.clearDisplay();
   
  }else timeshow();
       delay(50);
}*/
 /*//==============================呼吸灯===========================================
  * 
void savePower()
{
  int darkness= touchRead(T4);
   Serial.println(darkness);
  if(darkness < 40)
  {
    u8g2.setPowerSave(0);
   
  }else u8g2.setPowerSave(1);
}
   void breathLEDY(){
    for (int a=0; a<=255;a++)                //循环语句，控制PWM亮度的增加
   
  {
    analogWrite(LEDY,a);
    delay(8);                             //当前亮度级别维持的时间,单位毫秒            
  }
  for (int a=255; a>=0;a--)             //循环语句，控制PWM亮度减小
  {
    analogWrite(LEDY,a);
    delay(8);                             //当前亮度的维持的时间,单位毫秒  
  }
   }
  //==============================呼吸灯===========================================*/
