#include "Weight.h"

#define CALIBRATION 434.22 // キャリブレーション値

const int DT_PIN = 32;
const int SCK_PIN = 33;

static Weight *instance = NULL;

Weight::Weight(){}

Weight::Weight(FuncPtrInt f){
  instance = this;
  callBackEvent = f;
}

double Weight::measureWeight() {
  instance -> callBackEvent(MEASURING_WEIGHT);
  double prevWeight = 0;
  double nowWeight = 0;
  
  while(1) {
    nowWeight = scale.get_units(10);
    if(abs(nowWeight - prevWeight) < 0.1 && prevWeight > 1) {
       instance -> callBackEvent(COMPLETED_WEIGHT);
       Serial.println(nowWeight);
       return nowWeight; 
    }
    prevWeight = nowWeight;
    delay(100);
  }
}

void Weight::initWeight() {
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(CALIBRATION); 
  scale.tare();
  instance -> callBackEvent(INITIAL_COMPLETED_WEIGHT);
}
