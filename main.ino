/*
 Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND

 5V -> 5V
 GND -> GND
 CS -> 4
 MOSI -> 11
 SCK -> 13
 MISO -> 12

 14? -> LED+
 LED needs R = 200 Omega
*/

#include <HX711.h>
#include <SPI.h>
#include <SD.h>
#include <Vector.h>

#define calibration_factor -7050.0
#define zero_factor 8421804

#define SAMPLE_RATE  10

#define DOUT  3
#define CLK  2
#define CHIP_SELECT  4
#define LED  14

HX711 scale;
File dataFile;

enum Status {
  Ready,
  Activating,
  Init,
  Working,
} status;
bool LedStatus;

void setup() {
  status = Init;

  // Serial.begin(9600);
  pinMode(LED, OUTPUT);

  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.set_offset(zero_factor); //Zero out the scale using a previously known zero_factor

  if (!SD.begin(CHIP_SELECT)) {
    // Serial.println(F("SD card initialization failure. Exit..."));
    return;
  }

  dataFile = SD.open("data.txt", FILE_WRITE);
  if (!dataFile) {
    return;
  }

  status = Ready;
  LedStatus = false;
}

inline void HiLed () {
  if (!LedStatus) {
    digitalWrite(LED, HIGH);
    LedStatus = true;
  }
}

inline void LoLed () {
  if (LedStatus) {
    digitalWrite(LED, LOW);
    LedStatus = false;
  }
}

inline void writeData (unsigned long t, float f) {
  dataFile.print(t);
  dataFile.print(", ");
  dataFile.print(f);
  dataFile.print(", ");
}

unsigned long startTime = 0;
unsigned long countTime = 0;

void loop() {
  unsigned long t = millis();
  float value = scale.get_units();
  int valInt = short(value);
  switch (status) {
    case Ready:
      HiLed();
      if (valInt > 0) {
        LoLed();
        status = Activating;
        break;
      }
      delay(100);
      break;
    case Activating:
      if (valInt == 0) {
        status = Working;
        HiLed();
        delay(1000);
        LoLed();
      }
      break;
    case Working:
      if (valInt > 0) {
        if (startTime == 0) {
          startTime = t;
          countTime = t;
          HiLed();
        }
        if (t - countTime >= SAMPLE_RATE) {
          countTime = t;
          writeData(t - startTime, value);
        }
      }
      break;
  }
}
