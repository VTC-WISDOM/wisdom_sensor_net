# WISDOM Sensor Net
https://wisdomresearch.net
wisdomresearch@vermontstate.edu

Currently maintained by:
Evan Morse e.morse8686@gmail.com
Amelia Vlahogiannis amelia@ag2v.com
Sam Cowan

Welcome to the WiSDOM sensor net GitHub!
Here lives all of the test, example, and functional code for the WiSDOM node.

## Dependencies

[pico-sdk](https://github.com/raspberrypi/pico-sdk)
[picotool](https://github.com/raspberrypi/pico-sdk)

Debian based distributions:


```sudo apt install cmake python3 build-essential gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib```


Void linux:


```sudo xbps-install -Su cmake python3 base-devel cross-arm-none-eabi libusb-devel```

 - Install dependencies
 - Clone the pico-sdk repository and within it, run 
 ```git submodule update --init --recursive``` to ensure picotool works properly.
 - Set your PICO_SDK_PATH environment variable. For fish this is 
 ```set -Ux PICO_SDK_PATH /path/to/pico-sdk/```.
 - Clone the picotool repository and within it, run:
 ```
 mkdir build && cd build
 cmake ..
 sudo make install
 cp ../udev/60-picotool-rules /usr/lib/udev/rules.d/
 ```

## Setup 
This repository uses the [worm](https://github.com/VTC-WISDOM/worm) for submodules.
Rather than managing a chain of submodules with git, each level contains a worm.toml file.
For example, at the top level in this project:
```
  [[submodule]]
  remote = "https://github.com/VTC-WISDOM/wisdom_hal_extender.git"
  local = "whale/"
```
When `worm clone` is run, the linked repository for the whale will be cloned to the directory `whale/`
Similarly, the whale contains a worm.toml file linking submodules such as the eeprom and rtc drivers in.
The worm operates recursively. You only need to run `worm clone` once, and the entire project codebase will be pulled down.

## Contents
### docs
Documentation

### examples
Examples programs for various functions, sensors, and peripherals.

### templates
Templates for creating new projects from scratch

### field_tests
Primarily for the WiSDOM team's own use; stores actual implementations that have been or will be deployed.

### tools
Useful dev tools for setup, testing, and troubleshooting, such as the RTC sync tool.

### drivers
Drivers for sensors and peripherals - work is in progress to move these to the [WHALE(WiSDOM HAL Extender)](https://github.com/VTC-WISDOM/wisdom_hal_extender) or the WASP(WiSDOM Aggregated Sensor Protocols)

### libs
Exclusively software libraries. These are not hardware APIs.
