#include <Adafruit_NeoPixel.h>
#include "WS2812_Definitions.h"

#define LED_PIN 4
#define LED_COUNT 16
#define MAX_VOLUME_BASELINE 100
#define SELF_HEALING_THRESHOLD 1000 * 10
#define SELF_HEALING_STEP 50

Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int maxVolume = MAX_VOLUME_BASELINE;
unsigned long lastTimeWeHitMid = 0;

void setup() {
  Serial.begin(9600);
  leds.begin(); 
  clearLEDs();   
  leds.show();  
}

void loop() {
  int volumeValue = analogRead(0);   //connect mic sensor to Analog 0
  if(volumeValue > maxVolume) {
    maxVolume = volumeValue;
  }

  int numLEDsToLight = map(volumeValue, 0, maxVolume, 0, LED_COUNT);  
  // Self healing code
  if(numLEDsToLight >= LED_COUNT/2) {
    lastTimeWeHitMid = millis();
  }

  // If we haven't hit the middle of the LED in a while, decrease max value
  if(lastTimeWeHitMid > 0 && (millis() - lastTimeWeHitMid > SELF_HEALING_THRESHOLD)) {
    if (maxVolume > MAX_VOLUME_BASELINE + SELF_HEALING_STEP) {
      maxVolume = maxVolume - SELF_HEALING_STEP;
      delay(50);
    }
  }


  // Call the light up function
  visualizeSound(numLEDsToLight);

  delay(10); // Small delay for visual smoothness

}

void visualizeSound(int ledsToLight) {
  int rainbowScale = 192 / LED_COUNT;
  for(int i=0; i< LED_COUNT; i++) {
    if(i < ledsToLight) {
      leds.setPixelColor(i, rainbowOrder((rainbowScale * (i)) % 192));  // You can set any color here
    } else {
      // Apply a fade effect to the LEDs. This will dim them instead of turning them off instantly.
      uint32_t currentColor = leds.getPixelColor(i);
      int currentRed = (currentColor >> 16) & 0xFF;
      int currentGreen = (currentColor >> 8) & 0xFF;
      int currentBlue = currentColor & 0xFF;
      
      leds.setPixelColor(i, leds.Color(currentRed * 0.9, currentGreen * 0.9, currentBlue * 0.9));
    }
  }

  leds.show();
}

// Sets all LEDs to off, but DOES NOT update the display;
// call leds.show() to actually turn them off after this.
void clearLEDs()
{
  for (int i=0; i<LED_COUNT; i++)
  {
    leds.setPixelColor(i, 0);
  }
}

// Prints a rainbow on the ENTIRE LED strip.
//  The rainbow begins at a specified position. 
// ROY G BIV!
void rainbow(byte startPosition) 
{
  // Need to scale our rainbow. We want a variety of colors, even if there
  // are just 10 or so pixels.
  int rainbowScale = 192 / LED_COUNT;
  
  // Next we setup each pixel with the right color
  for (int i=0; i<LED_COUNT; i++)
  {
    // There are 192 total colors we can get out of the rainbowOrder function.
    // It'll return a color between red->orange->green->...->violet for 0-191.
    leds.setPixelColor(i, rainbowOrder((rainbowScale * (i + startPosition)) % 192));
  }
  // Finally, actually turn the LEDs on:
  leds.show();
}

// Input a value 0 to 191 to get a color value.
// The colors are a transition red->yellow->green->aqua->blue->fuchsia->red...
//  Adapted from Wheel function in the Adafruit_NeoPixel library example sketch
uint32_t rainbowOrder(byte position) 
{
  // 6 total zones of color change:
  if (position < 31)  // Red -> Yellow (Red = FF, blue = 0, green goes 00-FF)
  {
    return leds.Color(0xFF, position * 8, 0);
  }
  else if (position < 63)  // Yellow -> Green (Green = FF, blue = 0, red goes FF->00)
  {
    position -= 31;
    return leds.Color(0xFF - position * 8, 0xFF, 0);
  }
  else if (position < 95)  // Green->Aqua (Green = FF, red = 0, blue goes 00->FF)
  {
    position -= 63;
    return leds.Color(0, 0xFF, position * 8);
  }
  else if (position < 127)  // Aqua->Blue (Blue = FF, red = 0, green goes FF->00)
  {
    position -= 95;
    return leds.Color(0, 0xFF - position * 8, 0xFF);
  }
  else if (position < 159)  // Blue->Fuchsia (Blue = FF, green = 0, red goes 00->FF)
  {
    position -= 127;
    return leds.Color(position * 8, 0, 0xFF);
  }
  else  //160 <position< 191   Fuchsia->Red (Red = FF, green = 0, blue goes FF->00)
  {
    position -= 159;
    return leds.Color(0xFF, 0x00, 0xFF - position * 8);
  }
}
