#define BLYNK_TEMPLATE_ID "TMPL6UEaxh_Xj"
#define BLYNK_DEVICE_NAME "Home Automation"

#define BLYNK_FIRMWARE_VERSION    "0.1.0"
#define BLYNK_PRINT Serial

#define APP_DEBUG
#define USE_NODE_MCU_BOARD

#include "BlynkEdgent.h"

#include <OneWire.h>
#include<DallasTemperature.h> 
#include <NTPClient.h>
#include <WiFiUdp.h>

#define ONE_WIRE_BUS 2 // DS18B20 on arduino pin2 corresponds to D4 on physical board

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int temp_0;

int startTime1;
int endTime1;
int startTime2;
int endTime2;

WiFiUDP ntpUDP;
const long utcOffsetInSeconds = 19800;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

#define RELAY_PIN_1               5 // D1
#define RELAY_PIN_2               4 // D2   

#define VIRTUAL_RELAY_1                           V0
#define VIRTUAL_RELAY_2                           V1
#define TEMP_SENSOR                               V10

#define BUTTON_NUM                                 2
#define BUTTON_NO_PRESS                           -1
#define BUTTON_PLUG_1                              0
#define BUTTON_PLUG_2                              1

#define ON                                         0
#define OFF                                        1

unsigned short gusIsRelay_1_On = OFF;
unsigned short gusIsRelay_2_On = OFF;
unsigned short gusIsBut_Pressed = 0;

void setup()
{
  Serial.begin(115200);
  delay(100);
  
  BlynkEdgent.begin(); 
  DS18B20.begin();
  timeClient.begin();
  timer.setInterval(1000L, getSendData);
  
  pinMode(RELAY_PIN_1, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);

  vRelay_Control(BUTTON_PLUG_1, OFF);
  vRelay_Control(BUTTON_PLUG_2, OFF);
  vBlynk_Write_Button_State(BUTTON_PLUG_1, OFF);
  vBlynk_Write_Button_State(BUTTON_PLUG_2, OFF);
}

void loop() 
{
  short sBut = BUTTON_NO_PRESS;
  timer.run(); // Initiates SimpleTimer
  BlynkEdgent.run();
  
  if(sBut == BUTTON_PLUG_1)
  {
    if(gusIsBut_Pressed == 0)
    {
      if(gusIsRelay_1_On == ON)
      {
        vRelay_Control(sBut, OFF);
        vBlynk_Write_Button_State(sBut, OFF);
      }
      else 
      {
        vRelay_Control(sBut, ON);
        vBlynk_Write_Button_State(sBut, ON);
      }
      gusIsBut_Pressed = 1;
    }
  }
  else if(sBut == BUTTON_PLUG_2)
  {
    if(gusIsBut_Pressed == 0)
    {
      if(gusIsRelay_2_On == ON)
      {
        vRelay_Control(sBut, OFF);
        vBlynk_Write_Button_State(sBut, OFF);
      }
      else 
      {
        vRelay_Control(sBut, ON);
        vBlynk_Write_Button_State(sBut, ON);
      }
      gusIsBut_Pressed = 1; 
    }
  }
  else 
  {
    gusIsBut_Pressed = 0;
  }

  timeClient.update();
  int HH = timeClient.getHours();
  int MM = timeClient.getMinutes();
  int SS = timeClient.getSeconds();
  int server_time = 3600*HH + 60*MM + SS;

   if(startTime1 == server_time)
   {
    digitalWrite(RELAY_PIN_1, HIGH);
    Blynk.virtualWrite(VIRTUAL_RELAY_1, 1);
   }

   if(startTime2 == server_time)
   {
   digitalWrite(RELAY_PIN_2, HIGH);
   Blynk.virtualWrite(VIRTUAL_RELAY_2, 1);
   }

   if(endTime1 == server_time)
   {
   digitalWrite(RELAY_PIN_1, LOW);
   Blynk.virtualWrite(VIRTUAL_RELAY_1, 0);
   }

   if(endTime2 == server_time)
   {
   digitalWrite(RELAY_PIN_2, LOW);
   Blynk.virtualWrite(VIRTUAL_RELAY_2, 0);
   }

}

void getSendData()
{
  DS18B20.requestTemperatures(); 
  temp_0 = DS18B20.getTempCByIndex(0); // Sensor 0 will capture Temp in Celcius 
  Blynk.virtualWrite(TEMP_SENSOR, temp_0); //virtual pin V10
  //Serial.println(temp_0);
}

 
BLYNK_WRITE(VIRTUAL_RELAY_1)
{
  short sVirtual_Status = 0;
  sVirtual_Status = param.asInt();
  vRelay_Control(BUTTON_PLUG_1,sVirtual_Status);
}
BLYNK_WRITE(VIRTUAL_RELAY_2)
{
  short sVirtual_Status = 0;
  sVirtual_Status = param.asInt();
  vRelay_Control(BUTTON_PLUG_2,sVirtual_Status);
}

BLYNK_WRITE(V4){
 startTime1 = param[0].asInt();
 endTime1 = param[1].asInt();
 if(startTime1 == 0 && endTime1 == 0)
 {
  startTime1 = 999999;
  endTime1 = 999999;
 }
}

BLYNK_WRITE(V5){
 startTime2 = param[0].asInt();
 endTime2 = param[1].asInt();
 if(startTime2 == 0 && endTime2 == 0)
 {
  startTime2 = 999999;
  endTime2 = 999999;
 }
}

void vRelay_Control(short sBut, short sOnOff)
{
  if(sOnOff == ON)
  {
    if(sBut == BUTTON_PLUG_1)
    {
      digitalWrite(RELAY_PIN_1, LOW);
      gusIsRelay_1_On = ON;
    }
    else if(sBut == BUTTON_PLUG_2)
    {
      digitalWrite(RELAY_PIN_2, LOW);
      gusIsRelay_2_On = ON;
    }
  }
  else 
  {
    if(sBut == BUTTON_PLUG_1)
    {
      digitalWrite(RELAY_PIN_1, HIGH);
      gusIsRelay_1_On = OFF;
    }
    else if(sBut == BUTTON_PLUG_2)
    {
      digitalWrite(RELAY_PIN_2, HIGH);
      gusIsRelay_2_On = OFF;
    }
  }
}

short sStatus = BUTTON_NO_PRESS;

void vBlynk_Write_Button_State(short sBut, short sStatus)
{
  if(sBut == BUTTON_PLUG_1)
  {
    Blynk.virtualWrite(VIRTUAL_RELAY_1, sStatus);
  }
  else 
  {
    Blynk.virtualWrite(VIRTUAL_RELAY_2, sStatus);
  }
}
