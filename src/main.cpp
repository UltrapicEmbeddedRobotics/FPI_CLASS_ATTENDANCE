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


int FingerID=0,t1,t2;
//Fingerprint ID from the scanner 
bool device_Mode = false;                           // Default Mode Enrollment
bool firstConnect = false;
uint8_t uid;

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
HardwareSerial Serial1 (1);

// initializing the fingerprint snesor 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);






void setup() 
{
  Serial1.begin(115200);
  delay(100);
  uint8_t passcode[4];
  uint8_t correct = false;

      while(!correct)
      {
        while(!Serial1.available())
          ;
        Serial1.println(" \n\t\t FPI BIOMETRIC REGISTERATION SYSTEM");
        Serial1.println("PLS ENTER THE PASSCODE TO START");
          for(int i = 0; i < 4; i++)
          {
          passcode[i] = Serial.read();  
          }
          if(passcode[0] == 1 && passcode[1] == 2 && passcode[2] == 3 && passcode[3] == 4)
          
          correct = true;
          break;

      }
     //----INTIALIZE COMPLETE----
     void system_init();
  
}

void loop(){

}

//********************initialize attendance system ******************
void system_init()
{

     //-initiate OLED display--
     if(!screen.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
     Serial1.println(F("SSD1306 allocation failed"));
     for(;;); // Don't proceed, loop forever

     screen.clearDisplay();
  }

    // initialize Ethernet-
   Ethernet.init(ChipSelect_pin);
   Ethernet.begin(mac, ip); // initialize ethernet device 
   server.begin(); // server listening for client
   delay(500);

     if(Ethernet.linkStatus() == LinkON && client.connected())
     {
        Serial1.println("ethernet device found and connected");
        delay(1000);

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
     
       else if(Ethernet.linkStatus() == LinkOFF && Ethernet.hardwareStatus() == EthernetNoHardware)
       {
        Serial1.println("ethernet module not found");
        delay(1000);

        screen.clearDisplay();
        screen.setTextSize(1);             // Normal 1:1 pixel scale
        screen.setTextColor(WHITE);        // Draw white text
        screen.setCursor(0, 0);             // Start at top-left corner
        screen.print(F("E.connectfailed\n"));
        
       }
     


// -initialize Fingerprint module-
  
   finger.begin(57600);
   Serial1.println("\n\n FPI_Biometric_Enrollment");
   Serial.println("\n\n FPI_Biometric_Enrollment");

   if (finger.verifyPassword()) {
    Serial1.println("Found fingerprint sensor!");
    Serial.println("Found fingerprint sensor!");
    screen.clearDisplay();
    screen.drawBitmap( 34, 0, FinPr_valid_bits, FinPr_valid_width, FinPr_valid_height, WHITE);
  } else {
    Serial1.println("Did not find fingerprint sensor :(");
    Serial.println("Did not find fingerprint sensor :(");
    screen.clearDisplay();
    screen.drawBitmap( 32, 0, FinPr_failed_bits, FinPr_failed_width, FinPr_failed_height, WHITE);
    screen.display();
    while (1) { delay(1); }
  }
  finger.getTemplateCount();
  Serial1.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial1.println("Waiting for valid finger...");

   

}   

uint8_t getFingerprintEnroll() {

  Serial1.println("Ready to enroll a fingerprint!");
  Serial1.println("Please type in the UID # (from 1 to 127) you want to save this finger as...");
  uid = Serial1.read();
  if (uid == 0) {// ID #0 not allowed, try again!
     return;
  }
  Serial1.print("Enrolling UID #");
  Serial1.println(uid);

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(uid);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("UID "); Serial.println(uid);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(uid);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(uid);
  p = finger.storeModel(uid);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  
}
  
    

