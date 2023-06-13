#include <Arduino.h>
//#include <WiFi.h>
//#include <HTTPClient.h>
#include <inttypes.h>
#include <SPI.h>
#include <Wire.h>
#include "icons.h"
#include <Ethernet.h>

#include <SimpleTimer.h>
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h>     
#include <Adafruit_Fingerprint.h>


int FIngerID=0,t1,t2;
//Fingerprint ID from the scanner 
bool device_Mode = false;                           // Default Mode Enrollment
bool firstConnect = false;
uint8_t id;

#define Buzzer_pin PC14
#define ledRed  PA0
#define ledGreen  PA1



// biometric pins (serial2 pins on the esp32)
#define finger_rx PA2
#define finger_tx PA3

// W5500 ethernet module pins
#define Mosi_pin PA7
#define Miso_pin PA6
#define SCK_pin PA5
#define ChipSelect_pin PA4

// initializing OLED display screen
#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128
#define OLED_RESET 0

Adafruit_SSD1306 screen(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

 byte mac[] {0xde, 0xad,0xef,0xed};
    IPAddress ip(192,168,1,100);
   IPAddress ServerIP(127,0,0,1);
   uint16_t ServerPort = 80;

   EthernetClient client;
   EthernetServer server(ServerPort);



// initializing timer 
SimpleTimer timer;
HardwareSerial mySerial(2);

// initializing the fingerprint snesor 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);






void setup() 
{
  Serial.begin(115200);
  delay(100);

  void system_init();
  
}

void loop(){

}



//********************initialize attendance system ******************
void system_init()
{

     //-initiate OLED display--
     if(!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
     Serial.println(F("SSD1306 allocation failed"));
     for(;;); // Don't proceed, loop forever

     screen.clearDisplay();
  }

    // i-nitialize Ethernet-

   Ethernet.begin(mac, ip);
   server.begin();
   screen.print(" ");
   delay(500);

     if(Ethernet.linkStatus() == LinkON && client.connected())
     {
        screen.clearDisplay();
        screen.setTextSize(1);             // Normal 1:1 pixel scale
        screen.setTextColor(WHITE);        // Draw white text
        screen.setCursor(0, 0);             // Start at top-left corner
        screen.print(F("E.connect success\n"));
        screen.setCursor(0, 50);   
        screen.setTextSize(2);          
        screen.print(ip);
        screen.drawBitmap( 73, 10, Wifi_start_bits, Wifi_start_width, Wifi_start_height, WHITE);
        screen.display();
     }
     
       else if(Ethernet.linkStatus() == LinkOFF)
       {
        screen.clearDisplay();
        screen.setTextSize(1);             // Normal 1:1 pixel scale
        screen.setTextColor(WHITE);        // Draw white text
        screen.setCursor(0, 0);             // Start at top-left corner
        screen.print(F("E.connectfailed\n"));
       }
     


// -initialize Fingerprint module-
  
   finger.begin(57600);
   Serial.println("\n\n FPI_Biometric_Enrollment");

   if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
    screen.clearDisplay();
    screen.drawBitmap( 34, 0, FinPr_valid_bits, FinPr_valid_width, FinPr_valid_height, WHITE);
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    screen.clearDisplay();
    screen.drawBitmap( 32, 0, FinPr_failed_bits, FinPr_failed_width, FinPr_failed_height, WHITE);
    screen.display();
    while (1) { delay(1); }
  }

   

}   
    
  
    

