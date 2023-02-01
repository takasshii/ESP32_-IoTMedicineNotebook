#ifndef RFID_H_
#define RFID_H_

#include <SPI.h>
#include <MFRC522.h>

typedef enum {
  INITIAL_COMPLETED_RFID,
  MEASURING_RFID,
  COMPLETED_RFID,
} RFID_STATE;


class RFID {
  private:
    typedef void (*FuncPtrInt)(RFID_STATE);
    
  public:
    FuncPtrInt callBackEvent;
    RFID();
    RFID(FuncPtrInt f);
    void initRFID();
    String measureRFID();
};

#endif
