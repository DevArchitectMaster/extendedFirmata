#include <Firmata.h>

void stringDataCallback(char *stringData) {
  Firmata.sendString(stringData);
}

void setup() {
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  Firmata.attach(STRING_DATA, stringDataCallback);
  Firmata.begin();  
}

void loop() {
  while(Firmata.available()) {
    Firmata.processInput();
  }
}