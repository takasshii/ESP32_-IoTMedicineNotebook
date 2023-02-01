#ifndef DeviceState_H_
#define DeviceState_H_

#include "Network.h"
#include "Weight.h"
#include "RFID.h"

#include <ArxSmartPtr.h>

typedef enum {
  SETUP,
  SETUP_COMPETED,
  MEASURE,
  MEASURE_AND_SEND_COMPETED,
  ERROR_AND_INIT
} DEVICE_EVENT;

struct DEVICE_ALL_EVENT {
  Network_State_t networkState;
  WEIGHT_STATE weightState;
  RFID_STATE rfidState;
};

class DeviceState {
  private:
    friend void reducer();
    
    typedef void (*FuncPtrInt)(DEVICE_EVENT);
    
  public:
    FuncPtrInt callBackEvent;
    DeviceState();
    DeviceState(FuncPtrInt f);
    void initDeviceState();
    void initDevice();
    void actionCreator(DEVICE_ALL_EVENT event);
};

#endif
