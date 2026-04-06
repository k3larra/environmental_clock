# Environmental Clock

This solution turns an 8-pixel NeoPixel-compatible strip on `D2` into a simple environmental clock for the Arduino Nano 33 BLE Sense.

## Chosen environment input

I used the built-in `APDS9960` proximity sensor. That makes the clock respond to how close a hand or object is to the board, which fits the brief of reflecting the Nano's immediate surroundings.

## Behaviour

- One LED advances once per second around the 8-pixel strip, acting like a clock hand.
- The active LED colour updates continuously from the proximity sensor, not only when the hand moves.
- Far away objects produce cooler colours.
- Close objects shift the colour toward warmer colours.
- A dim trailing pixel is left behind to make the motion easier to read.

## Non-blocking design

The sketch uses `millis()` timers throughout:

- `CLOCK_INTERVAL_MS` controls when the clock hand advances.
- `SENSOR_INTERVAL_MS` controls how often the proximity sensor is sampled.

Because these timers run independently, the LED colour can change dynamically between clock steps. No `delay()` is used in the main clock logic.

## Libraries

- `Arduino_APDS9960`
- `Adafruit_NeoPixel`

## Wiring

- NeoPixel strip `DIN` -> `D2`
- NeoPixel strip `5V`/`VIN` -> suitable power supply
- NeoPixel strip `GND` -> Arduino `GND`

For a very small strip, powering from the Nano can work during testing, but a separate suitable supply is safer for longer use.

## Notes

- The sketch targets `arduino:mbed_nano:nano33ble`.
- If the proximity sensor does not start, the strip shows a repeating red error indicator.
