#include "DeviceState.h"

bool networkConnected = false;
bool networkDisconnected = false;
bool firebsaeConnected = false;
bool firebsaeDisconnected = false;
bool firebaseSendCompleted = false;
bool initialCompletedWeight = false;
bool measureingWeight = false;
bool completedWeight = false;
bool initialCompletedRFID = false;
bool measureingRFID = false;
bool completedRFID = false;

static DeviceState *instance = NULL;

DeviceState::DeviceState(){}

DeviceState::DeviceState(FuncPtrInt f){
  instance = this;
  callBackEvent = f;
}

// Stateの状態をもとに新たなActionを作成する
void reducer() {
  if(networkConnected && firebsaeConnected) {
    instance -> callBackEvent(SETUP_COMPETED);
    return;
  }
  if(measureingWeight || measureingRFID) {
    instance -> callBackEvent(MEASURE);
    return;
  }
  if(completedWeight && completedRFID && firebaseSendCompleted) {
    instance -> callBackEvent(MEASURE_AND_SEND_COMPETED);
    return;
  }
  if(networkDisconnected || firebsaeDisconnected) {
    instance -> callBackEvent(ERROR_AND_INIT);
    return;
  }
}

void DeviceState::initDevice() {
  instance -> callBackEvent(SETUP);
}

void DeviceState::initDeviceState() {
  Serial.println("initDeviceState");
  networkConnected = false;
  networkDisconnected = false;
  firebsaeConnected = false;
  firebsaeDisconnected = false;
  firebaseSendCompleted = false;
  initialCompletedWeight = false;
  measureingWeight = false;
  completedWeight = false;
  initialCompletedRFID = false;
  measureingRFID = false;
  completedRFID = false;
}

// Actionを発生させてStoreに保存
void DeviceState::actionCreator(DEVICE_ALL_EVENT event) {
  if(event.networkState == NETWORK_CONNECTED) {
    networkConnected = true;
    Serial.println("networkConnected : true");
  } else if(event.networkState == NETWORK_DISCONNECTED) {
    networkDisconnected = true;
    Serial.println("networkDisconnected : true");
  } else if(event.networkState == FIREBASE_CONNECTED) {
    firebsaeConnected = true;
    Serial.println("firebsaeConnected : true");
  } else if(event.networkState == FIREBASE_DISCONNECTED) {
    firebsaeDisconnected = true;
    Serial.println("firebsaeDisconnected : true");
  } else if(event.networkState == FIREBASE_SEND_COMPLETED) {
    firebaseSendCompleted = true;
    Serial.println("firebaseSendCompleted : true"); 
  } else if(event.weightState == INITIAL_COMPLETED_WEIGHT) {
    initialCompletedWeight = true;
    Serial.println("initialCompletedWeight : true");
  } else if(event.weightState == MEASURING_WEIGHT) {
    measureingWeight = true;
    Serial.println("measureingWeight : true");
  } else if(event.weightState == COMPLETED_WEIGHT) {
    completedWeight = true;
    Serial.println("completedWeight : true");
  } else if(event.rfidState == INITIAL_COMPLETED_RFID) {
    initialCompletedRFID = true;
    Serial.println("initialCompletedRFID : true");
  } else if(event.rfidState == MEASURING_RFID) {
    measureingRFID = true;
    Serial.println("measureingRFID : true");
  } else if(event.rfidState == COMPLETED_RFID) {
    completedRFID = true;
    Serial.println("completedRFID : true");
  }
  reducer();
}
