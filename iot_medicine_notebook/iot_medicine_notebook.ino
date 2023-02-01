#include "Network.h"
#include "LED.h"
#include "Weight.h"
#include "RFID.h"
#include "DeviceState.h"

Network *network;
LED *led;
Weight *weight;
RFID *rfid;
DeviceState *deviceState;

TaskHandle_t thp[2];
QueueHandle_t xQueue_1;
QueueHandle_t xQueue_2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initLED(); 
  initDeviceState();
  initWeight();
  initRFID();
  initNetwork();

  xQueue_1 = xQueueCreate(10,16);
  xQueue_2 = xQueueCreate(10,32);

  //スレッドの準備
  xTaskCreatePinnedToCore(sendWeight, "sendWeight", 8192, NULL, 1, &thp[0], 1);
  xTaskCreatePinnedToCore(sendFirebase, "sendFirebase", 8192, NULL, 2, &thp[1], 0);
}


void loop() {
  // put your main code here, to run repeatedly:
  String result;
  result = rfid -> measureRFID();
  xQueueSend(xQueue_2, &result, 0);
  delay(100);
}


// 重量を測る
void sendWeight(void *args) {
  double result;
  while(1) {
    Serial.println("Start MeasureWeight");
    result = weight -> measureWeight();
    if(result != NULL) {
      xQueueSend(xQueue_1, &result, 0);
    }
    delay(100);
  }
}

// Firebaseに値を送信する
void sendFirebase(void *args) {
  double weightResult;
  String rfid;
  while (1) {
    /* メッセージ受信待ち */
    xQueueReceive(xQueue_1, &weightResult, portMAX_DELAY);
    xQueueReceive(xQueue_2, &rfid, portMAX_DELAY);

    Serial.print("[Core0a] xQueueReceive:");
    Serial.print(weightResult);
    Serial.print(rfid);

    if(weightResult != NULL & rfid != NULL) {
      network->firestoreDataUpdate(weightResult, rfid);
      delay(1);
    }
    delay(1);
  }
}

// LEDの初期化
void initLED() {
  led = new LED();
  led -> initLED();
}

// Networkの初期化
void initNetwork() {
  void (*ptr)(Network_State_t) = &networkEvent;
  network = new Network(ptr);
  network -> initWiFi();  
}

//　NetworkEventのCallback
void networkEvent(Network_State_t event){
  DEVICE_ALL_EVENT state;
  switch(event){
    case NETWORK_CONNECTED:
      state.networkState = NETWORK_CONNECTED;
      deviceState -> actionCreator(state);
      break;
    case NETWORK_DISCONNECTED:
      state.networkState = NETWORK_DISCONNECTED;
      deviceState -> actionCreator(state);
      break;
    case FIREBASE_CONNECTED:
      state.networkState = FIREBASE_CONNECTED;
      deviceState -> actionCreator(state);
      break;
    case FIREBASE_DISCONNECTED:  
      state.networkState = FIREBASE_DISCONNECTED;
      deviceState -> actionCreator(state);
      break;
    case FIREBASE_SEND_COMPLETED:
      state.networkState = FIREBASE_SEND_COMPLETED;
      deviceState -> actionCreator(state);
      break;
    default: break;
  }
}

// Weightの初期化
void initWeight() {
  void (*ptr)(WEIGHT_STATE) = &weightEvent;
  weight = new Weight(ptr);
  weight -> initWeight();  
}

//　WeightEventのCallback
void weightEvent(WEIGHT_STATE event){
  DEVICE_ALL_EVENT state;
  Serial.println(event);
  switch(event){
    case INITIAL_COMPLETED_WEIGHT:
      state.weightState = INITIAL_COMPLETED_WEIGHT;
      deviceState -> actionCreator(state);
      break;
    case MEASURING_WEIGHT:  
      state.weightState = MEASURING_WEIGHT;
      deviceState -> actionCreator(state);
      break;
    case COMPLETED_WEIGHT:
      state.weightState = COMPLETED_WEIGHT;
      deviceState -> actionCreator(state);
      break;
    default: break;
  }
}

// RFIDの初期化
void initRFID() {
  void (*ptr)(RFID_STATE) = &rfidEvent;
  rfid = new RFID(ptr);
  rfid -> initRFID();  
}

//　RFIDEventのCallback
void rfidEvent(RFID_STATE event){
  DEVICE_ALL_EVENT state;
  Serial.println(event);
  switch(event){
    case INITIAL_COMPLETED_RFID:
      state.rfidState = INITIAL_COMPLETED_RFID;
      deviceState -> actionCreator(state);
      break; 
    case MEASURING_RFID:  
      state.rfidState = MEASURING_RFID;
      deviceState -> actionCreator(state);
      break;
    case COMPLETED_RFID:
      state.rfidState = COMPLETED_RFID;  
      deviceState -> actionCreator(state);
      break;
    default: break;
  }
}

// RFIDの初期化
void initDeviceState() {
  void (*ptr)(DEVICE_EVENT) = &dispatcher;
  deviceState = new DeviceState(ptr);
  deviceState -> initDevice();
  deviceState -> initDeviceState();
}

// 全体のイベント制御
void dispatcher(DEVICE_EVENT event) {
  deviceState -> initDeviceState();
  switch(event){
    case SETUP:
      led -> changeLEDColor(BLUE_LOOP);
      break;
    case SETUP_COMPETED:
      led -> changeLEDColor(BLUE_LIGHT);
      break; 
    case MEASURE:
      led -> changeLEDColor(BLUE_LOOP);
      break;
    case MEASURE_AND_SEND_COMPETED:  
      led->changeLEDColor(GREEN_LIGHT_SHORT);
      break;
    case ERROR_AND_INIT:  
      led->changeLEDColor(RED_LIGHT);
      break;
    default: break;
  }
}
