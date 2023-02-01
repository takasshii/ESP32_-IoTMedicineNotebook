#include "LED.h"

#define DIN_PIN 16 // Digital Pinの場所
#define LED_COUNT 35 // LEDの数

#define LED_LIGHT_TIME 5000 // LEDを点灯させる時間

static LED *instance = NULL;
static Adafruit_NeoPixel pixels(LED_COUNT, DIN_PIN, NEO_GRB + NEO_KHZ800);

LED::LED() {
  instance = this;
}

void lightLED(int R, int G, int B) {
  for(int i=0; i<LED_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(R, G, B));
    pixels.show();
  }
}

void lightLEDShort(int R, int G, int B) {
  unsigned long start_time = millis();
  while((millis() - start_time) < LED_LIGHT_TIME) {
    lightLED(R,G,B);
  }
}

void loopLED(int R, int G, int B) {
   for(int i=0; i<LED_COUNT; i++) {
      pixels.clear();
      pixels.setPixelColor(i, pixels.Color(R, G, B));
      pixels.show();
      delay(10);
  }
}


void LED::initLED() {
  pixels.begin();
}

void LED::changeLEDColor(LED_COLOR color) {
  switch(color){
    case BLUE_LIGHT:
       lightLED(0,0,128);
      break;
    case BLUE_LOOP:
       loopLED(0,0,128);
      break; 
    case GREEN_LIGHT_SHORT:  
       lightLEDShort(0,128,0);
      break;
    case RED_LIGHT:  
       lightLED(128,0,0);
      break;
    default: break;
  } 
}
