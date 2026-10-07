#include <Arduino.h>

#ifdef PI
#undef PI
#endif

#include "App.h"

ExatasApp app;

void setup() {
    app.begin();
}

void loop() {
    app.loop();
}
