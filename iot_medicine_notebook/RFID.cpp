#include "RFID.h"

#define RST_PIN   26  
#define SS_PIN    5  
#define MISO_PIN  19 
#define MOSI_PIN  23 
#define SCK_PIN   18 

static MFRC522 mfrc522(SS_PIN, RST_PIN);
static RFID *instance = NULL;

RFID::RFID(){}

RFID::RFID(FuncPtrInt f){
  instance = this;
  callBackEvent = f;
}

void RFID::initRFID() {
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  instance -> callBackEvent(INITIAL_COMPLETED_RFID);
}

String RFID::measureRFID() {
  instance -> callBackEvent(MEASURING_RFID);
  while(1) {
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
          continue;
      }
      if ( ! mfrc522.PICC_ReadCardSerial()) {
          continue;
      }
  
      String strBuf[mfrc522.uid.size];
      for (byte i = 0; i < mfrc522.uid.size; i++) {
          strBuf[i] =  String(mfrc522.uid.uidByte[i], HEX);
          if(strBuf[i].length() == 1){
            strBuf[i] = "0" + strBuf[i];
          }
      }
  
      String strUID = strBuf[0] + strBuf[1] + strBuf[2]+ strBuf[3];
      
      if(strUID != NULL) {
        instance -> callBackEvent(COMPLETED_RFID);  
      }
      
      delay(1);
      Serial.println(strUID);
      return strUID;
  }
}
