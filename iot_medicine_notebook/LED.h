#ifndef LED_H_
#define LED_H_

#include <Adafruit_NeoPixel.h>

typedef enum {
  BLUE_LIGHT,
  BLUE_LOOP,
  GREEN_LIGHT_SHORT,
  RED_LIGHT  
} LED_COLOR;

class LED {
  private:
    friend void lightLED(int R, int G, int B);
    friend void lightLEDShort(int R, int G, int B);
    friend void loopLED(int R, int G, int B);
    
  public:
    LED();
    void initLED();
    void changeLEDColor(LED_COLOR color);
};

#endif
