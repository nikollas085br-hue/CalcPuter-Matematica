#include <Arduino.h>

#ifdef PI
#undef PI
#endif

#include "App.h"

void setup() {
    Serial.begin(115200);
    delay(100);

    App::begin();
}

void loop() {
    App::update();
}
