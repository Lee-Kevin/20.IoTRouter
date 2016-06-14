/*
 * File: routerBox_Arduino.ino
 * 
 * Author: Lambor
 * Date: 2016-03-10
 * Modify：Jiankai Li
 * Date: 2016-05-31
 * In this demo, mt7688Duo as a router, its net speed and WiFi users' number 
 * will be determined and save to files("/tmp/netSpeed", "/tmp/wifi_user_number").
 *
 * Mertirials: RGB LED Strip with 10 leds
 *             mt7688Duo
 *             Arduino Breakout for LinkIt Smart 7688Duo
 * 
 */
#include <Process.h>
#include "Adafruit_NeoPixel.h"
#include <string.h>
#include "HDC1000.h"
#include <FileIO.h>
#include <avr/wdt.h>
#include <TimerOne.h>

#include "Adafruit_Thermal.h"
#include "SoftwareSerial.h"

#define PIXIEL_PIN 6
#define PIXIEL_NUM 6
#define USER_MAXNUM   6

#define INTERVAL      1          // Time interval unit s
#define INTERVAL_DATA 5          // Time interval unit s
#define RELAY_PIN     5          // 

#define TX_PIN 4 // Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 3 // Arduino receive   GREEN WIRE   labeled TX on printer
SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor


unsigned long previousTime = 0;  // define the last time  unit s
unsigned long previousTimeData = 0;  // define the data last time  unit 

/* Define the state machine status */

enum Status 
{
    Standby          =  0,         
    UpdateNetStatus  =  1,
    UpdateSensorData =  2,
    UpdateRelay      =  3,
	UpdatePrinter    =  4,
	Unknow           =  5,
};
typedef enum Status Systemstatus;
Systemstatus WorkingStatus;

struct SensorData 
{
    float Temperature   = 0;
    float Humidity      = 0;
};
typedef struct SensorData SensorData_t;
SensorData_t HomeSensor;
SensorData_t *pHomeSensor;


Adafruit_NeoPixel pixels= Adafruit_NeoPixel(PIXIEL_NUM, PIXIEL_PIN, NEO_GRB + NEO_KHZ800);

enum routerInfo { user_num=0, net_speed=1 };

#define BUF_SIZE  8
char *g_Buffer;
char buff[30];
int g_BufferIndex;

volatile uint8_t current_color = 0;
volatile uint8_t last_color = 0;

HDC1000 mySensor;
  
void bridgeCatFile(int routerInfo) {
  Process p;
  int index;
  
  g_Buffer = (char *)malloc(BUF_SIZE);
  memset(g_Buffer, '\0', BUF_SIZE);
  
  p.begin("cat");
  if(routerInfo == user_num){
    p.addParameter("/tmp/wifi_user_number");    
  } else if (routerInfo == net_speed) {
    p.addParameter("/tmp/netSpeed");
  }
  p.run();
  
  while (p.available() > 0) {
    char c = p.read();
    g_Buffer[index++] += c;
  }
  g_Buffer[index++] += '\0';  
  g_BufferIndex = index;
}

uint16_t getNetSpeed() {
  String data = "";
  int index = 0;
  int number = 0;
  
  bridgeCatFile(net_speed);
  index = g_BufferIndex;
  
  for(int i=0;i< g_BufferIndex-1; i++){
    data += (String)g_Buffer[i];    
  }
  number = data.toInt();
  free(g_Buffer);
  
  Serial.print("Speed: ");
  Serial.println(number);
  
  return number;
}

uint16_t getWiFiUser() {
  String data = "";
  int index = 0;
  int number = 0;
  
  bridgeCatFile(user_num);
  index = g_BufferIndex;
  for(int i=0;i< g_BufferIndex-1; i++){
    data += (String)g_Buffer[i];    
  }
  number = data.toInt();
  free(g_Buffer);  
  
  Serial.print("User: ");
  Serial.println(number);
  
  return number;
  
}

void pixels_Action(uint16_t speed, uint16_t users) {
  uint16_t maxSpeed = 1024;
  uint16_t minSpeed = 0;
  uint8_t colorRange = 255;
  uint8_t color;
  uint32_t _color;  
  if (speed < 10) {
      speed = 10;
  }
  current_color = map(speed > maxSpeed ? maxSpeed : speed, 0, maxSpeed, 0, colorRange);
  color=last_color;
  do {
    current_color > last_color ? (color++):(color--);
    _color = pixels.Color(color, 0, 255 - color);  
    for (int i=0; i<USER_MAXNUM; i++) {
        if (i < users) {
            pixels.setPixelColor(i, _color);
        } else {
            pixels.setPixelColor(i, 0);
        }
    }
  pixels.show();
  delay(10);      
  } while((current_color > last_color ? (color < current_color):(color > current_color)));
  
  last_color = current_color;
  
}

void setup() {
  Serial.begin(9600);
  Serial.println("power on!");
  // Initialize led strip
  pixels.begin(); 
//  pixels.clear();
  pixels.setBrightness(20);

  // Initialize Bridge
  Bridge.begin();
  // Initialize the FileSystem
  FileSystem.begin();
  // Initialize the HDC1000
  mySensor.begin();
  // Initialize the Relay
  pinMode(RELAY_PIN,OUTPUT);
  
  // Initialize the printer
  mySerial.begin(19200);
  printer.begin();
  printer.setDefault();
  
  
  previousTime = millis()/1000;
  previousTimeData = millis()/1000 - random(3,10);
  
  bridgeMkdir();
  bridgeSaveData("relay",0);
  delay(1000);
  wdt_enable(WDTO_2S);
 
  Timer1.initialize(1000000); // set a timer of length 1000000 microseconds 1 second
  Timer1.attachInterrupt( timerIsrFeedFog ); // attach the service routine here
  wdt_reset();
}

void loop() {
	unsigned long currentTime = millis()/1000;
	int speed;
	int user_num;
	if (currentTime - previousTime >= INTERVAL) {

		previousTime = currentTime;
		WorkingStatus = UpdateNetStatus;
	}
	if (currentTime - previousTimeData >= INTERVAL_DATA) {

		previousTimeData = currentTime;
		WorkingStatus = UpdateSensorData;
	}

	switch(WorkingStatus){
	case UpdateNetStatus:
		speed = getNetSpeed();
		user_num = getWiFiUser();
        Timer1.detachInterrupt();
        wdt_reset();
		pixels_Action(speed, user_num);
        wdt_reset();
        Timer1.attachInterrupt( timerIsrFeedFog ); // attach the service routine here
		
		// Serial.println("Net Status");
		WorkingStatus = UpdateRelay;
		break;
	case UpdateSensorData:
		bridgeMkdir();
		HomeSensor.Temperature = mySensor.getTemp();
		HomeSensor.Humidity    = mySensor.getHumi();
		// Serial.print("Temp: ");
		// Serial.println(HomeSensor.Temperature);
		// Serial.print("Humi: ");
		// Serial.println(HomeSensor.Humidity);
		
		bridgeSaveData("temp",HomeSensor.Temperature);
		bridgeSaveData("humi",HomeSensor.Humidity);

		WorkingStatus = Unknow;
		break;
	case UpdateRelay:
		uint8_t command;
		command = bridgeGetRelayCom();
		if (1 == command) {
			OpenRelay();
		} else {
			CloseRelay();
		}
		// Serial.print("Relay Command: ");
		// Serial.println(bridgeGetRelayCom());
		WorkingStatus = UpdatePrinter;
		break;
	case UpdatePrinter:
		command = bridgeGetPrintCom();
        bridgeClearPrintCom();
		if (1 == command) {
            
            printer.wake(); 
			Serial.println("------------------------Open Printer--------------------------");
			// printerTest();
			bridgeGetPrintMessage();
			// bridgeClearPrintCom();
            printer.sleep();
		}
		Serial.println("UpdatePrinter");
		WorkingStatus = Unknow;
		
	case Unknow:
		break;
	default:
		break; 
	}
	
}

void bridgeMkdir() {
	Process p;
	p.begin("mkdir");
	p.addParameter("/tmp/Sensor");
	p.runAsynchronously();
}

void bridgeSaveData(char *sname,float data) {
	sprintf(buff,"/tmp/Sensor/%s",sname);
	File script = FileSystem.open(buff,FILE_WRITE);
	script.print(data);
	script.close();
}
/* Get the relay command from 7688 */
uint8_t bridgeGetRelayCom() {
	Process p;
	String str;
	uint8_t command;
	p.begin("cat");
	p.addParameter("/tmp/Sensor/relay");
	p.run();
	while (p.available() > 0) {
		char c = p.read();
		str += c;
	}
	command = str.toInt();
	return command;
	
}
/* Get the printer command from 7688 */
uint8_t bridgeGetPrintCom() {
	Process p;
	String str;
	uint8_t command;
	p.begin("cat");
	p.addParameter("/tmp/printFlag");
	p.run();
	while (p.available() > 0) {
		char c = p.read();
		str += c;
	}
	command = str.toInt();
	return command;
	
}
/* Clear the printer command  */
void bridgeClearPrintCom() {
	File script = FileSystem.open("/tmp/printFlag",FILE_WRITE);
	script.print(0);
	script.close();
}
/* Test the printer */
void printerMessage(String data) {  
  uint8_t i,index;
  String str;
  printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'  
  printer.justify('C');
  printer.println("MESSAGE");
  printer.setDefault();
  wdt_reset();
  index = data.length()/80;
  Serial.println("-----------Print the message-----------------");
  Serial.println(data.length());
  for ( i=0; i<=index; i++) {
      if (i == index) {
          str = data.substring(i*80,(i*80 + data.length()%80));
      } else {
          str = data.substring(i*80,(i+1)*80);
      }
      
      printer.print(str);
      Serial.println("------------------------------------------------------");
      Serial.println(str);
      wdt_reset();
      
  }
  
  wdt_reset();
  printer.feed(4);  
}

void bridgeGetPrintMessage() {
	Process p;
	String str;
	p.begin("cat");
	p.addParameter("/tmp/message");
	p.run();
	while (p.available() > 0) {
		char c = p.read();
		str += c;
	}
    Serial.println(str);
    
    printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'  
    printer.justify('C');
    printer.println("MESSAGE");
    printer.setDefault();
    Serial.println("-----------Print the message-----------------");
    Serial.println(str.length());

    printer.print(str);
    printer.feed(4);  

}

void timerIsrFeedFog()
{
    wdt_reset();
    Serial.println("------------Time ISR");
}


void OpenRelay() {
	digitalWrite(RELAY_PIN, HIGH);
}

void CloseRelay() {
	digitalWrite(RELAY_PIN, LOW);
}
