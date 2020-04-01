#include <Arduino.h>
#include <Automaton.h>

#define PIN_BUTTON       3
#define PIN_LED_YELLOW  10
#define PIN_LED_GREEN   11

Atm_led ledYellow;
Atm_led ledGreen;
Atm_button button;
Atm_fan buttonPressed;

void setup() {
  Serial.begin(9600);
  ledYellow.begin(PIN_LED_YELLOW).blink(200, 200);
  ledGreen.begin(PIN_LED_GREEN);
  buttonPressed.begin();

  button.begin(PIN_BUTTON)
    .onPress(buttonPressed, buttonPressed.EVT_INPUT)
    .onRelease(ledGreen, ledYellow.EVT_OFF);

  buttonPressed
    .onInput(ledYellow, ledYellow.EVT_TOGGLE_BLINK)
    .onInput(ledGreen, ledYellow.EVT_ON);

  button.trace(Serial);
}

void loop() {
  automaton.run();
}
