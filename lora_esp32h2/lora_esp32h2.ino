/*
  LoRa Duplex communication wth callback

  Sends a message every half second, and uses callback
  for new incoming messages. Implements a one-byte addressing scheme,
  with 0xFF as the broadcast address.

  Note: while sending, LoRa radio is not listening for incoming messages.
  Note2: when using the callback method, you can't use any of the Stream
  functions that rely on the timeout, such as readString, parseInt(), etc.

  created 28 April 2017
  by Tom Igoe
*/
#include <SPI.h>              // include libraries
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_TMP117.h> 

//define the pins used by the LoRa transceiver module
#define SCK 4
#define MISO 0
#define MOSI 5
#define SS 1
#define RST 12
#define DIO0 22

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST -1 // Reset pin # (or -1 if sharing Arduino reset pin) 
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
TMP117 sensor; // Initalize sensor

String outgoing;              // outgoing message
byte msgCount = 0;            // count of outgoing messages

int lastSendTime = 0;        // last send time
long send_interval = 2000;          // interval between sends



void setup() {
  
  Serial.begin(115200);
  Wire.begin(2,3);
  Serial.println(F("SSD1306 allocation failed"));
  
  
 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

    Serial.println("TMP117 Example 1: Basic Readings");
  if (sensor.begin(0x48) == true) // Function to check if the sensor will correctly self-identify with the proper Device ID/Address
  {
    Serial.println("Begin");
  }
  else
  {
    Serial.println("Device failed to setup- Freezing code.");
    while (1); // Runs forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("ESP32 LoRa 1 (Sender)");
  display.display();



  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);  
  if (!LoRa.begin(BAND))
  {
    Serial.println("Starting LoRa failed!");
     //display.setCursor(0,10);
     //display.print("Starting LoRa failed!");
    // display.display();
    while (1);
  }
  
  LoRa.setSyncWord(0xF3);  
 
  Serial.println("LoRa Initializing OK!");
  //display.setCursor(0,10);
  //display.print("LoRa Initializing OK!");
  //display.display();
}


void loop() {
  
  //delay(100);
  float tempC = sensor.readTempC();
  //float tempC = 100.2; 
  String this_temp = String(tempC);  
  
  if (millis() - lastSendTime > send_interval)
  {       
    sendMessage(this_temp);
    Serial.println("Sending " + this_temp);
    lastSendTime = millis();                
  }
    
  lora_receive(this_temp);
}

void sendMessage(String outgoing){
  LoRa.beginPacket();
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  
}

void lora_receive(String this_temp){
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0) return;
            // if there's no packet, return

  while (LoRa.available()) {    

  String incoming = LoRa.readString();

  Serial.print("yrah.........");

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("ESP32-H2 temp:");
  display.setCursor(0,10);    
  display.print(this_temp);
  display.print(" ");
  display.write((char)248);
  display.println("C");

    display.setCursor(0,30);
    display.println("ESP32-c6 temp:");
    display.setCursor(0,40);    
    display.print(incoming);
    display.print(" ");
    display.write((char)248);
    display.println("C");
    display.display();
  
  
  
  
}

}
