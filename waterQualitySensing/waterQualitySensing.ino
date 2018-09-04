//SD CARD
#include <SPI.h>
#include <SD.h>
#define FILENAME "data.txt"

//TEMP SENSOR
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define TDS_PIN A0
#define TURBIDITY_PIN A1
#define DISSOLVED_OXYGEN_PIN A2
#define PH_PIN A3

void setup() {
  //Serial
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  //SD CARD
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  writeTextToFile("time,temperature,TDS-a0,a1,a2,a3");

  //TEMP SENSOR
  // Start up the library
  sensors.begin();
}

void loop() {

  //TEMP SENSOR
  sensors.requestTemperatures();

  writeDataToFile(millis(),
                  sensors.getTempCByIndex(0),
                  analogReadVolts(TDS_PIN),
                  analogReadVolts(TURBIDITY_PIN),
                  analogReadVolts(DISSOLVED_OXYGEN_PIN),
                  readPH(PH_PIN));

  //PING LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  //DELAY
  delay(1000);
}

double analogReadVolts(int pin) {
  return analogRead(pin) * (5.0 / 1023.0);
}

/**
   Write the sensor data to the file
*/
void writeDataToFile(long time, double temperature , double a0, double a1, double a2, double a3) {
  File myFile = SD.open(FILENAME, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(time);
    myFile.print(",");
    myFile.print(temperature);
    myFile.print(",");
    myFile.print(a0);
    myFile.print(",");
    myFile.print(a1);
    myFile.print(",");
    myFile.print(a2);
    myFile.print(",");
    myFile.print(a3);
    myFile.println("");
    myFile.close();

    Serial.print(time);
    Serial.print(",");
    Serial.print(temperature);
    Serial.print(",");
    Serial.print(a0);
    Serial.print(",");
    Serial.print(a1);
    Serial.print(",");
    Serial.print(a2);
    Serial.print(",");
    Serial.print(a3);
    Serial.println("");
  }
}

/**
   Write a text message to the file
*/
void writeTextToFile(char * str) {
  File myFile = SD.open(FILENAME, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(str);
    myFile.println("");
    myFile.close();
  }
}


double readPH(int pin) {
  unsigned long int avgValue;  //Store the average value of the sensor feedback
  float b;
  int buf[10], temp;
  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buf[i] = analogRead(pin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)               //take the average value of 6 center sample
    avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6; //convert the analog into millivolt
  phValue = 3.5 * phValue;                  //convert the millivolt into pH value
  return phValue;
}
