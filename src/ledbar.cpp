#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

#include "ledbar.h"
#include "measurements.h"
#include <Adafruit_NeoPixel.h>

static Adafruit_NeoPixel *strip = NULL;
static int (*shouldReadMoisture)() = NULL;

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip->Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip->numPixels(); i++) { // For each pixel in strip->..
    strip->setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip->show();                          //  Update strip to match
    vTaskDelay(wait / portTICK_PERIOD_MS);
  }
}

void whiteOverRainbow(int whiteSpeed, int whiteLength) {

  if(whiteLength >= strip->numPixels()) whiteLength = strip->numPixels() - 1;

  int      head          = whiteLength - 1;
  int      tail          = 0;
  int      loops         = 3;
  int      loopNum       = 0;
  uint32_t lastTime      = millis();
  uint32_t firstPixelHue = 0;

  for(;;) { // Repeat forever (or until a 'break' or 'return')
    for(int i=0; i<strip->numPixels(); i++) {  // For each pixel in strip->..
      if(((i >= tail) && (i <= head)) ||      //  If between head & tail...
         ((tail > head) && ((i >= tail) || (i <= head)))) {
        strip->setPixelColor(i, strip->Color(0, 0, 0, 255)); // Set white
      } else {                                             // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / strip->numPixels());
        strip->setPixelColor(i, strip->gamma32(strip->ColorHSV(pixelHue)));
      }
    }

    strip->show(); // Update strip with new contents
    // There's no delay here, it just runs full-tilt until the timer and
    // counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    if((millis() - lastTime) > whiteSpeed) { // Time to update head/tail?
      if(++head >= strip->numPixels()) {      // Advance head, wrap around
        head = 0;
        if(++loopNum >= loops) return;
      }
      if(++tail >= strip->numPixels()) {      // Advance tail, wrap around
        tail = 0;
      }
      lastTime = millis();                   // Save time of last movement
    }
  }
}

void pulseWhite(uint8_t wait) {
  for(int j=0; j<256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip->fill(strip->Color(0, 0, 0, strip->gamma8(j)));
    strip->show();
    vTaskDelay(wait / portTICK_PERIOD_MS);
  }

  for(int j=255; j>=0; j--) { // Ramp down from 255 to 0
    strip->fill(strip->Color(0, 0, 0, strip->gamma8(j)));
    strip->show();
    vTaskDelay(wait / portTICK_PERIOD_MS);
  }
}

int idle_animation(int wait, int rainbowLoops) {
  int fadeVal=0, fadeMax=100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) {

    for(int i=0; i<strip->numPixels(); i++) { // For each pixel in strip->..

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip->numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip->numPixels());

      // strip->ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip->setPixelColor(i, strip->gamma32(strip->ColorHSV(pixelHue, 255,
        255 * fadeVal / fadeMax)));
    }

    strip->show();

    uint32_t notification = ulTaskNotifyTake(pdTRUE, wait / portTICK_PERIOD_MS);
    if (notification > 0) {
      return 1;
    }

    fadeVal = fadeMax; // Interim loop, make sure fade is at max
  }
  return 0;
}
// Declare our NeoPixel strip object:
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
void ledbar_initialize(LedbarConfig_t *config) {
  strip = new Adafruit_NeoPixel(config->count, config->pin, NEO_GRB + NEO_KHZ800);
  strip->begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip->show();  // Turn OFF all pixels ASAP
  strip->setBrightness(config->brightness); // Set BRIGHTNESS to about 1/5 (max = 255)

  shouldReadMoisture = config->shouldReadMoisture;
}

void ledbar_destroy() {
  delete strip;
}

void display_percent(int percent) {
  int okLeds = strip->numPixels() * percent / 100;
  Serial.println(percent);

  strip->clear();

  for (int i = 0; i < okLeds; i++) {
    strip->setPixelColor(i, strip->Color(0, 255, 0));
  }

  strip->setPixelColor(okLeds - 1, strip->Color(255, 255, 0));

  for (int i = okLeds; i < strip->numPixels(); i++) {
    strip->setPixelColor(i, strip->Color(255, 0, 0));
  }

  strip->show();
}

void ledbar_task(void *params) {
  int should_display_moisture;
  while (true) {
    should_display_moisture = idle_animation(25, 30);
    if (should_display_moisture) {
      display_percent(measurements_read_moisture());
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
  vTaskDelete(NULL);
}

void measurements_monitor_task(void *params) {
  TaskHandle_t led_task_handle = (TaskHandle_t) params;

  while (true) {
    if (shouldReadMoisture()) {
      xTaskNotifyGive(led_task_handle);
    }

    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
}

void ledbar_task_start() {
  if (strip == NULL) {
    return;
  }
  TaskHandle_t led_task_handle;
  xTaskCreate(ledbar_task, "ledbar_idle", 128, NULL, 2, &led_task_handle);
  xTaskCreate(measurements_monitor_task, "ledbar_measurements", 128, led_task_handle, 1, NULL);

  // Fill along the length of the strip in various colors...
  // colorWipe(strip.Color(255,   0,   0)     , 50); // Red
  // colorWipe(strip.Color(  0, 255,   0)     , 50); // Green
  // colorWipe(strip.Color(  0,   0, 255)     , 50); // Blue
  // colorWipe(strip.Color(  0,   0,   0, 255), 50); // True white (not RGB
  // white)

  // whiteOverRainbow(75, 5);

  // pulseWhite(5);
}

