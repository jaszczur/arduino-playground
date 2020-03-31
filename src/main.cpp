#include <Arduino.h>
#include <Automaton.h>

#define PIN_BUTTON 3
#define PIN_LED    10

Atm_led led;
Atm_button button;

void setup() {
  Serial.begin(9600);
  led.begin(PIN_LED).blink(200, 200);
  button.begin(PIN_BUTTON).onPress(led, led.EVT_TOGGLE_BLINK);
}

void loop() {
  automaton.run();
}
