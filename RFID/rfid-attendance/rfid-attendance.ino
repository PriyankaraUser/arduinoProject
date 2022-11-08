#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>

#define RST_PIN  D3
#define SS_PIN   D4
#define BUZZER   D2

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  
MFRC522::StatusCode status;      


int blockNum = 2;  

byte bufferLen = 18;
byte readBlockData[18];

String card_holder_name;
const String sheet_url = "https://script.google.com/macros/s/AKfycbybHP1D7szypVriPFISnbxFh95-ceKkXv4EpjV69lXXTlaMf5iM1fjzVSTAuzNjKtIi/exec?name=";

// fingerprint
const uint8_t fingerprint[20] = {0x8D, 0xC5, 0x65, 0x82, 0x10, 0x71, 0x12, 0x6B, 0x24, 0x9B, 0x99, 0x8C, 0x1F, 0xA9, 0x0C, 0xD1, 0x0E, 0x07, 0xC0, 0xA4};

#define WIFI_SSID "Dialog 4G"
#define WIFI_PASSWORD "TQ04BGLLR0L"



void setup()
{

  Serial.begin(9600);

  Serial.println();
  Serial.print("Connecting to AP");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  pinMode(BUZZER, OUTPUT);

  SPI.begin();

}


 void loop()
{

  mfrc522.PCD_Init();

  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}

  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}

  Serial.println();
  Serial.println(F("Reading last data from RFID..."));
  ReadDataFromBlock(blockNum, readBlockData);

  Serial.println();
  Serial.print(F("Last data in RFID:"));
  Serial.print(blockNum);
  Serial.print(F(" --> "));
  for (int j=0 ; j<16 ; j++)
  {
    Serial.write(readBlockData[j]);
  }
  Serial.println();

  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
  delay(200);
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);

  if (WiFi.status() == WL_CONNECTED) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    client->setFingerprint(fingerprint);

    card_holder_name = sheet_url + String((char*)readBlockData);
    card_holder_name.trim();
    Serial.println(card_holder_name);

    HTTPClient https;
    Serial.print(F("[HTTPS] begin...\n"));


    if (https.begin(*client, (String)card_holder_name)){

      Serial.print(F("[HTTPS] GET...\n"));
      
      int httpCode = https.GET();

      if (httpCode > 0) {

        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

      }

      else 
      {Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());}

      https.end();
      delay(1000);
    }

    else {
      Serial.printf("[HTTPS} Unable to connect\n");
    }

  }

}




void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{ 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK){
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }

  else {
    Serial.println("Authentication success");
  }

  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  else {
    Serial.println("Block was read successfully");  
  }

}
