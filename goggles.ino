#include <Button.h>
#include <Adafruit_NeoPixel.h>

const int BUTTON = 0;
const int LENS = 1;
const int NUM_PIXELS = 32;
const int MAX_BRIGHTNESS = 200;
const int MIN_BRIGHTNESS = 10;
const int NUM_SPIN = 3;
const int MAX_SPIN_POS = NUM_PIXELS / 2 / NUM_SPIN;


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, LENS);
Button button = Button(BUTTON, true, true, 25);

uint32_t currentColor = 0x215475;
uint8_t currentWheelPos = 0;
uint8_t currentBrightness = MIN_BRIGHTNESS;
uint8_t currentSpinPos = 0;
uint16_t state = 0;
bool modeChangePrimed = false;

const uint8_t INTERVAL = 25;
unsigned long previousMillis = 0;
unsigned long lastPressAt;
unsigned long now;
uint8_t quickPress = 0;

enum MODE {
  modeSteady, modeBreath, modeSpin, modeRainbowCycle
} currentMode = modeBreath;

void changeMode() {
  switch (currentMode) {
    case modeSteady: currentMode = modeSpin; break;
    case modeSpin: currentMode = modeRainbowCycle; break;
    case modeRainbowCycle: currentMode = modeSteady; break;
  }
  state = 0;
  flash(5);
}



void setup() {
  // put your setup code here, to run once:
  pixels.begin();
  pixels.setBrightness(MIN_BRIGHTNESS);
  setAllPixels(currentColor);
  pixels.show();
}

void setAllPixels(uint32_t color) {
  for(int i = 0; i < NUM_PIXELS; ++i) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}

void flash(int brightness) {
  if (!brightness) brightness = 200;
  pixels.setBrightness(brightness);
  setAllPixels(random(0, 0xffffff));
  delay(250);
  pixels.setBrightness(currentBrightness);
  setAllPixels(currentColor);
}

void cycleColor() {
  if (!canAct(INTERVAL)) return;
  currentWheelPos += 1;
  if (currentWheelPos > 255) currentWheelPos = 0;
  currentColor = Wheel(currentWheelPos & 255);
  setAllPixels(currentColor);
}

void spin() {
  if (!canAct(60)) return;
  setAllPixels(0x000000);
  for (int i = 0; i < MAX_SPIN_POS; ++i) {
    if (i == currentSpinPos) {
      for (int j = 0; j < NUM_SPIN; ++j) {
        int offset = i + (j * MAX_SPIN_POS);
        pixels.setPixelColor(offset, currentColor);
        pixels.setPixelColor(altEyePos(offset), currentColor);
      }
      
    }
  }
  if (++currentSpinPos == MAX_SPIN_POS) {
    currentSpinPos = 0;
  }
}

uint8_t altEyePos(uint8_t eyePos) {
  if (eyePos <= 10) return (NUM_PIXELS - 1) - 5 - eyePos;
  else return NUM_PIXELS - 1 - (eyePos - 11);
}

void loop() {
  now = millis();
  button.read();
  if (button.pressedFor(5000)) {
    cycleColor();
    modeChangePrimed = false;
  } else if (button.pressedFor(2000)) {
    if (!modeChangePrimed) {
      flash(50);
      flash(50);
      modeChangePrimed = true;
    }
  } else if (button.wasPressed()) {
    if (now - lastPressAt < 1500) {
      if (++quickPress > 2) {
        cycleBrightness();
      }
    } else {
      quickPress = 0;
      flash(0);
    }
    lastPressAt = now;

  } else if (button.wasReleased()) {
    if (modeChangePrimed) {
      changeMode();
      modeChangePrimed = false;
    }
  } else {
    switch (currentMode) {
      case modeRainbowCycle: rainbowCycle(); break;
      case modeSpin: spin(); break;
      default: break;
    }
  }
  if (currentBrightness != 0) pixels.show();
}



bool canAct(uint8_t interval) {
  if (now - previousMillis > interval) {
    previousMillis = now;
    return true;
  }
  return false;
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

void rainbowCycle() {
  if (!canAct(INTERVAL)) return;
  for(int i=0; i< NUM_PIXELS / 2; i++) {
    pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + state) & 255));
    pixels.setPixelColor(altEyePos(i), Wheel(((i * 256 / pixels.numPixels()) + state) & 255));
  }
  pixels.show();
  state++;
  if (state == 256) {
    state = 0;
  }
}

void cycleBrightness() {
  if (currentBrightness == 0) {
    currentBrightness = MIN_BRIGHTNESS;
  } else if (currentBrightness == MIN_BRIGHTNESS) {
    currentBrightness = 70;
  } else if (currentBrightness == 70) {
    currentBrightness = 140;
  } else if (currentBrightness == 140) {
    currentBrightness = MAX_BRIGHTNESS;
  } else {
    currentBrightness = 0;
  }
  pixels.setBrightness(currentBrightness);
  pixels.show();
}


