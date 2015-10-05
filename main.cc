#include <Button.h>
#include <Adafruit_NeoPixel.h>

const int BUTTON = 0;
const int LENS = 1;
const int NUM_PIXELS = 32;
const int MAX_BRIGHTNESS = 100;
const int MIN_BRIGHTNESS = 5;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, LENS);
Button button = Button(BUTTON, false, false, 25);

uint32_t currentColor = 0x9B0DBE;
uint8_t currentWheelPos = 0;
uint8_t currentBrightness = MIN_BRIGHTNESS;
bool breathingUp = true;

const unsigned long INTERVAL = 25;
unsigned long previousMillis = 0;

enum MODE {
  modeSteady, modeBreath, modeRainbowCycle
} currentMode = modeSteady;



void setup() {
  // put your setup code here, to run once:
  pixels.begin();
  pixels.setBrightness(currentBrightness);
  setAllPixels(currentColor);
  pixels.show();
}

void setAllPixels(uint32_t color) {
  currentColor = color;
  for(int i = 0; i < NUM_PIXELS; ++i) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}

void flash() {
  uint32_t restoreColor = currentColor;
  pixels.setBrightness(254);
  setAllPixels(random(0, 0xffffff));
  delay(250);
  pixels.setBrightness(currentBrightness);
  setAllPixels(restoreColor);
}

void cycleColor() {
  if (!canAct()) return;
  currentWheelPos += 1;
  if (currentWheelPos > 255) currentWheelPos = 0;
  setAllPixels(Wheel(currentWheelPos));
}

void loop() {
  button.read();
  if (button.pressedFor(5000)) {
    cycleColor();
  } else if (button.wasPressed()) {
    flash();
  } else {
    switch (currentMode) {
      case modeBreath: breath(); break;
      case modeRainbowCycle: rainbowCycle(25); break;
      default: break;
    }
  }
  
  pixels.show();
}

bool canAct() {
  if (millis() - previousMillis > INTERVAL) {
    previousMillis = millis();
    return true;
  }
  return false;
}

void breath() {
  if (!canAct()) return;
  if (currentBrightness <= MIN_BRIGHTNESS) breathingUp = true;
  if (currentBrightness >= MAX_BRIGHTNESS) breathingUp = false;
  currentBrightness += (breathingUp ? 1 : -1);
  pixels.setBrightness(currentBrightness);
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, c);    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}
