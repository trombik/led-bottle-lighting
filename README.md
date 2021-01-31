# Bottle Lighting

A lighting system to illuminate bottles. Supported bottle diameter is 100 mm.

## Requirements

### Electrical components

- ESP8266 or ESP32 development board
- Low `Vth` N-channel MOSFET
- some passive components
- Universal board
- Touch sensor
- 12V LED strip or SPI-based addressable LED strip (`SK9822`, or `APA102`)
- 18 or 20 AWG power cable
- 3D printer

### Tools

- Soldering iron and a solder wire
- Oscilloscope (optional for debugging)
- Multimeter (optional for debugging)
- A breadboard (optional for debugging)
- A small LED (optional for debugging)

### Software

- [`git`](https://git-scm.com/)
- Slicer software, such as [`cura`](https://ultimaker.com/software/ultimaker-cura)
- [`esp-idf`](https://github.com/espressif/esp-idf) or [ESP8266 RTOS SDK](https://github.com/espressif/ESP8266_RTOS_SDK)

## Features

Dims the LED by PWM.

Brightness can be controlled by a touch switch.

Schedules on/off in cron(5) format.

## Schematic

See a `kicad` project in [schematic](schematic).

## Firmware

See the `esp-idf` project in [`src`](src).

## License

ISC. See [LICENSE](LICENSE).

## Authors

- [Tomoyuki Sakurai](https://github.com/trombik)
