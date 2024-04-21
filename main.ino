/*
 Last modified: 2025 04 18

 Arduino pin 2 -> HX711 SCK (CLK)
 3 -> DT (D_OUT)
 5V -> VCC
 GND -> GND

 5V -> 5V
 GND -> GND
 CS -> 4
 MOSI -> 11
 SCK -> 13
 MISO -> 12

 5? -> LED+
 9? -> Switch
 LED needs R = 200 Omega
 Switch needs R = 10K Omega
*/

#include <HX711.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <TM1650.h>

#define calibration_factor -11181
#define zero_factor 8421804

#define SAMPLE_RATE  50

#define DOUT  3
#define CLK  2
#define CHIP_SELECT  53
#define LED  LED_BUILTIN  //5
#define SWITCH 9

HX711 scale;
File dataFile;
TM1650 display;

enum Status {
  Ready,
  Activating,
  Init,
  Working,
  Done
} status;
bool LedStatus;

/*
* Error List
* 1: SD card init failed
* 2: File cannot be read (e.g. the file is read-only)
*/
void error (int id) {
  String str = String("E") + String(id);
  if (getK(id) == 1) {
    str = str + String("  ");
  } else if (getK(id) == 2) {
    str = str + String(" ");
  }
  display.displayString(str.c_str());
}

void setup() {
  status = Init;

  /* System Setup */
  Serial.begin(9600);
  Serial.println("Starting...");
  pinMode(LED, OUTPUT);
  pinMode(SWITCH, INPUT);

  /* Display Setup */
  Wire.begin(); //Join the bus as master
  display.init();
  display.displayOn();
  display.setBrightnessGradually(0);

  /* HX711 Setup */
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();	//Reset the scale to 0
 // scale.set_offset(zero_factor); //Zero out the scale using a previously known zero_factor

  /* SD Setup */
  if (!SD.begin(CHIP_SELECT)) {
    Serial.println(F("SD card initialization failure. Exit..."));
    error(1);
    delay(1000);
    exit(1);
    return;
  }
  dataFile = SD.open("data.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println(F("Data file cannot be open. Exit..."));
    error(2);
    delay(1000);
    exit(1);
    return;
  }

  Serial.println("Started~");

  status = Ready;
  LedStatus = false;
  display.displayString("8888");
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

inline int getK (int val) {
  if (val < 0) return 0;
  if (0 <= val && val < 10) return 1;
  if (10 <= val && val < 100) return 2;
  if (100 <= val && val < 1000) return 3;
  if (1000 <= val && val < 10000) return 4;
}

inline String getDisplayStr (int value) {
  String originNum = String(value);
  for (int i = 0; i < 4 - getK(value); i ++) {
    originNum = String("0") + originNum;
  }
  return originNum;
}

// Default as value (g).
// if value > 9999 -> output as XX.XX (kg)
inline void displayValue (int value) {
  if (0 <= value && value < 10000) {
    display.displayString(getDisplayStr(value).c_str());
  } else if (value < 1000000) {
    display.displayString(getDisplayStr(value / 10).c_str());
    display.setDot(0, true);
  } else {
    error(3);
  }
}

inline void writeData (unsigned long t, float f) {
  dataFile.print(t);
  dataFile.print(", ");
  dataFile.print(f);
  dataFile.print(", ");

  // Serial.print("time: ");
  // Serial.print(t);
  // Serial.print(" value: ");
  // Serial.println(f);
}

unsigned long startTime = 0;
unsigned long countTime = 0;

void loop() {
  unsigned long t = millis();
  double value = scale.get_units() * 100;
  int valInt = int(value);
  if (false) { // DEBUG -> set it to true !!!
    Serial.print(status);
    Serial.print(" ");
    Serial.println(value);
  }
  switch (status) {
    case Ready:
      HiLed();
      if (valInt > 10) {
        LoLed();
        status = Activating;
        break;
      }
      delay(100);
      break;
    case Activating:
      if (valInt <= 0) {
        status = Working;
        display.displayString("3   ");
        HiLed();
        delay(1000);
        LoLed();
        delay(200);
        display.displayString("2   ");
        HiLed();
        delay(1000);
        LoLed();
        delay(200);
        display.displayString("1   ");
        HiLed();
        delay(1000);
        LoLed();
        display.displayString("   0");
     } else {
        display.displayString("0000");
      }
      break;
    case Working:
      // if (digitalRead(SWITCH) == HIGH) {
      //   LoLed();
      //   dataFile.close();
      //   status = Done;
      //   break;
      // }
      if (valInt > 0) {
        if (startTime == 0) {
          HiLed();
          startTime = t;
          countTime = t;
        }
        if (t - countTime >= SAMPLE_RATE) {
          countTime = t;
          writeData(t - startTime, value);
          displayValue(valInt);
        }
      }
      if (valInt < -1000) {
        LoLed();
        dataFile.println("");
        dataFile.close();
        display.displayString("    ");
        Serial.println("Stop");
        exit(0);
      }
      break;
    case Done:
      break;
  }
}
