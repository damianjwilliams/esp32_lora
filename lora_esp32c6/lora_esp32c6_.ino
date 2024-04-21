/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SparkFun_TMP117.h> // Used to send and recieve specific information from our sensor

TMP117 sensor; // Initalize sensor


//define the pins used by the LoRa transceiver module
#define SCK 3
#define MISO 2
#define MOSI 11
#define SS 6
#define RST 4
#define DIO0 5

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 915E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST -1 // Reset pin # (or -1 if sharing Arduino reset pin) 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//packet counter
int i = 0;
int arrows [4] = { 24, 26, 25 ,27 };


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

const char str[] = "temp here: ";
char valueStr[25]; 
float temperature;


void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);

  //reset OLED display via software
  //pinMode(OLED_RST, OUTPUT);
  //digitalWrite(OLED_RST, LOW);
  //delay(20);
  //digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(0,1);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  Serial.println("TMP117 Example 1: Basic Readings");
  if (sensor.begin(0x4A) == true) // Function to check if the sensor will correctly self-identify with the proper Device ID/Address
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
  display.print("LORA receiver ");
  display.display();
  
  Serial.println("LORA receiver test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
}

void loop() {

  temperature = sensor.readTempC();
  String temp_to_send = String(temperature);
  //snprintf(valueStr, 10, "%d", temperature);
  //char result[20]; // Assumes that the concatenated string will be no longer than 20 characters
  //strcpy(result, str);
  //strcat(result, valueStr);

  


  int packetSize = LoRa.parsePacket();
  if (packetSize) {
     //received a packet
  Serial.print("Received packet '");
  }

    // read packet
    while (LoRa.available()) {
      String LoRaData = LoRa.readString();
      Serial.println(LoRaData);

      
   

    //String LoRaDataStr = "1";
    //int converted_string;
    //converted_string = LoRaDataStr.toInt();
    display.cp437(true); 

    display.clearDisplay();
    display.setTextSize(1);
    
    display.setCursor(0,0);
    display.println("ESP32-C6 temp:");
    display.setCursor(0,10);    
    display.print(temperature);
    display.print(" ");
    display.write((char)248);
    display.println("C");

    display.setCursor(0,30);
    display.println("ESP32-H2 temp:");
    display.setCursor(0,40);    
    display.print(LoRaData);
    display.print(" ");
    display.write((char)248);
    display.println("C");
    
    
    display.setCursor(0,55);
    //display.println("Temperature there:");
    if(i>3){
      i=0;   
    }
    display.write(arrows[i]);
             
    display.display();

    LoRa.beginPacket();    
    LoRa.print(temp_to_send);
    LoRa.endPacket();
    Serial.println("return packet sent!");
    Serial.println(arrows[i]);

    i++;


    }

  

   //delay(200);

   
}

  


  
