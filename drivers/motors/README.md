# ESP32 Stepper driver tests

Testing ESP32 with 8825/4988 Stepper Driver library on Nema stepper motors like 17HS19 or similars.

## Libraries dependencies install

Before install [PlatformIO](http://platformio.org/) open source ecosystem for IoT development, then install libraries:

``` javascript
platformio lib install 1272 1018
```

## Compile and firmware upload

``` javascript
platformio run -e esp32dev --target upload
``` 

## Basic Connection

![alt text][drv8825]

[drv8825]:https://a.pololu-files.com/picture/0J4233.600.png?665d623ba84232de64511d8aa6644836  "General connection for DRV8825"

## Usage:


