#include <Arduino.h>
#include <limits.h>

#define PIN_BUTTON 3
#define PIN_LED    10

class ButtonFSM {};

class LedFSM {};

struct State {
  bool ledBlinking = false;
  bool ledOn = false;
  unsigned long nextBlink = ULONG_MAX;

  unsigned long buttonPressedTime = 0;
  unsigned long buttonUnpress = 0;

  unsigned long now = 0;
};

State state;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
}

void loop() {
  // read state from sensors
  bool buttonPressed = !digitalRead(PIN_BUTTON);
  state.now = millis();

  // update current state
  if (state.buttonPressedTime == 0) {
    if (buttonPressed) {
      Serial.println("arming");
      state.buttonPressedTime = state.now + 40;
    }
  } else if (state.now >= state.buttonPressedTime) {
    if (buttonPressed) {
      Serial.println("button is still pressed. it's for real");
      state.ledBlinking = !state.ledBlinking;

      if (state.ledBlinking) {
        state.nextBlink = state.now;
      } else {
        state.nextBlink = ULONG_MAX;
        state.ledOn = false;
      }
    } else {
      Serial.println("nope");
    }
    state.buttonPressedTime = 0;
  } else {
    Serial.println("hold");
  }

  if (state.now >= state.nextBlink && state.ledBlinking) {
    state.ledOn = !state.ledOn;
    state.nextBlink = state.now + 500;
  }

  // set actuators
  digitalWrite(PIN_LED, state.ledOn);

  // reset state
  //memcpy(prevState, state, sizeof(State));
}
