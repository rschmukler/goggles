#include <Adafruit_NeoPixel.h>

const int BUTTON = 1;
const int LENS = 0;
const int LONG_HOLD_TIME = 2000;
const int NUM_PIXELS = 16;

int buttonPressed = 0;
int lastButtonState = 0;
int ledState = LOW;
int buttonPressStartTime;
bool longHoldAlreadyTriggered = false;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, LENS);

void setup() {
  uint32_t startColor = 0x9B0DBE;
  // put your setup code here, to run once:
  pinMode(BUTTON, INPUT);
  pixels.begin();
  pixels.setBrightness(255);
  setAllPixels(startColor);
  pixels.show();
}

void setAllPixels(uint32_t color) {
  for(int i = 0; i < NUM_PIXELS; ++i) {
    pixels.setPixelColor(i, color);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  checkButton();
  if (longHoldStart()) {
//    blinkLED();
  }
  if (longHoldRelease()) {
//    toggleLED();
  }
  pixels.show();
  lastButtonState = buttonPressed;
}

//void blinkLED() {
//  int restoreLed = ledState;
//  digitalWrite(LED, LOW);
//  delay(300); 
//  digitalWrite(LED, HIGH);
//  digitalWrite(LED, restoreLed);
//}

void checkButton() {
  buttonPressed = digitalRead(BUTTON);
  if (buttonPressed != lastButtonState) {
    if (buttonPressed == HIGH) {
      longHoldAlreadyTriggered = false;
      buttonPressStartTime = millis();
    }
  }
}

bool longHoldStart() {
  if (!longHoldAlreadyTriggered && buttonPressed == HIGH && (millis() - buttonPressStartTime) > LONG_HOLD_TIME) {
    longHoldAlreadyTriggered = true;
    return true;
  }
  return false;
}

bool longHoldRelease() {
  return buttonPressed != lastButtonState && (millis() - buttonPressStartTime) > LONG_HOLD_TIME;
}

//void toggleLED() {
//  if (ledState == HIGH) {
//    ledState = LOW;
//  } else {
//    ledState = HIGH;
//  }
//  digitalWrite(LED, ledState); 
//}
