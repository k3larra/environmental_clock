#include <Arduino_APDS9960.h>
#include <Adafruit_NeoPixel.h>

namespace {
const uint8_t PIXEL_PIN = 2;
const uint8_t PIXEL_COUNT = 8;

const unsigned long CLOCK_INTERVAL_MS = 1000;
const unsigned long SENSOR_INTERVAL_MS = 60;

Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

unsigned long lastClockTick = 0;
unsigned long lastSensorTick = 0;

uint8_t currentPixel = 0;
int filteredProximity = 0;

uint32_t wheelColor(uint8_t position) {
  position = 255 - position;
  if (position < 85) {
    return pixels.Color(255 - position * 3, 0, position * 3);
  }

  if (position < 170) {
    position -= 85;
    return pixels.Color(0, position * 3, 255 - position * 3);
  }

  position -= 170;
  return pixels.Color(position * 3, 255 - position * 3, 0);
}

void renderClock() {
  pixels.clear();

  const uint8_t hue = map(filteredProximity, 0, 255, 160, 0);
  const uint32_t activeColor = wheelColor(hue);
  const uint32_t tailColor = pixels.Color(
    (uint8_t)((activeColor >> 16) & 0xFF) / 8,
    (uint8_t)((activeColor >> 8) & 0xFF) / 8,
    (uint8_t)(activeColor & 0xFF) / 8
  );

  pixels.setPixelColor(currentPixel, activeColor);
  pixels.setPixelColor((currentPixel + PIXEL_COUNT - 1) % PIXEL_COUNT, tailColor);
  pixels.show();
}

void updateProximity() {
  if (!APDS.proximityAvailable()) {
    return;
  }

  int rawProximity = APDS.readProximity();
  rawProximity = constrain(rawProximity, 0, 255);

  // Smooth the sensor reading to reduce jitter while keeping updates responsive.
  filteredProximity = (filteredProximity * 3 + rawProximity) / 4;
  renderClock();
}

void advanceClockHand() {
  currentPixel = (currentPixel + 1) % PIXEL_COUNT;
  renderClock();
}

void showSensorError() {
  static unsigned long lastBlinkTick = 0;
  static uint8_t errorPixel = 0;

  if (millis() - lastBlinkTick < 120) {
    return;
  }

  lastBlinkTick = millis();
  pixels.clear();
  pixels.setPixelColor(errorPixel, pixels.Color(255, 0, 0));
  pixels.show();
  errorPixel = (errorPixel + 1) % PIXEL_COUNT;
}
}  // namespace

void setup() {
  pixels.begin();
  pixels.setBrightness(32);
  pixels.show();

  Serial.begin(115200);
}

void loop() {
  static bool sensorReady = false;
  static bool triedSensorInit = false;

  if (!triedSensorInit) {
    triedSensorInit = true;
    sensorReady = APDS.begin();
    renderClock();
  }

  if (!sensorReady) {
    showSensorError();
    return;
  }

  const unsigned long now = millis();

  if (now - lastSensorTick >= SENSOR_INTERVAL_MS) {
    lastSensorTick = now;
    updateProximity();
  }

  if (now - lastClockTick >= CLOCK_INTERVAL_MS) {
    lastClockTick = now;
    advanceClockHand();
  }
}
