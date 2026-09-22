# MaxBotix_Library

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.6338415.svg)](https://doi.org/10.5281/zenodo.6338415)

Arduino library to enable control and communication with MaxBotix ultrasonic rangefinders using a software serial implementation.

This Arduino library will enable the microcontroller to control and read output from a MaxBotix ultrasonic rangefinder. It works using software serial, and can control, read data from, and average over multiple measurements from the rangefinder.

*Boss: "I just heard that light travels faster than sound. I'm wondering if I should shout when I speak, just so my lips appear to sync-up with my words."*
*Dilbert (thought): "A little knowledge can be a ridiculous thing."*
&mdash;Scott Adams

**Installation:** included in [NorthernWidget-libraries](https://github.com/NorthernWidget/NorthernWidget-libraries).

```cpp
#include <Maxbotix.h>

Maxbotix rangefinder;

void setup() {
    Serial.begin(9600);
    rangefinder.begin(10);                // average over 10 pings per reading
    Serial.println(rangefinder.getHeader());
}

void loop() {
    Serial.println(rangefinder.getString());
    delay(1000);
}
```

**Full API reference:** https://docs.northernwidget.com/MaxBotix_Library/
