#include <Arduino.h>
// GPS
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
// SD
#include <SPI.h>
#include <SD.h>
// DS18B20
#include <OneWire.h>
#include <DallasTemperature.h>

// PC
byte serialCom = 0;
#define monitor_speed 115200
#define serialComSwitch 4

// LED
#define errorLED 3

// SIGNAL
#define endOP 7

// GPS
static const int RXPin = 8, TXPin = 9;
SoftwareSerial ss(RXPin, TXPin);
TinyGPSPlus gps;
#define gps_speed 9600

// SD
File dataFile;
char* fileName = "data.txt";
#define chipSelect 10

// DS18B20
#define ONE_WIRE_BUS 5
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress termometroInterno = {0x28, 0xAD, 0x1D, 0x23, 0x09, 0x00, 0x00, 0xEE};
DeviceAddress termometroAgua = {0x28, 0xAA, 0xA6, 0x35, 0x13, 0x13, 0x02, 0x40};
DeviceAddress termometroAire = {0x28, 0xEE, 0x73, 0x2D, 0x14, 0x16, 0x02, 0xC4};

// Functions
void SerialComMode();
void dataGetMode();
void openDataFile();
void listDataFile();
void readDataFile();
void deleteDataFile();
void closeDataFile();
void saveGpsData();
void saveTempData();
void savePressureData();
void saveUvData();
float readLinealAnalogSensorMv(int analogPin);
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);

void setup()
{
  Serial.begin(monitor_speed);
  ss.begin(gps_speed);
  sensors.begin();

  pinMode(chipSelect, OUTPUT);
  pinMode(serialComSwitch, INPUT);
  pinMode(errorLED, OUTPUT);
  pinMode(endOP, OUTPUT);
  digitalWrite(endOP, LOW);

  /* Serial.println();
  Serial.println(F("termoDaQ V1.0"));
  Serial.println(F("[OSHW] VE000001"));
  Serial.println(F("https://github.com/mc-ireiser/termoDaQ"));
  Serial.println(); */

  // Test card
  if (!SD.begin(chipSelect))
  {
    Serial.println(F("Error: Verifique tarjeta SD"));
    digitalWrite(errorLED, HIGH);
    // Wait forever
    while (1){}
  }

  // Serial.println(F("Tarjeta SD inicializada"));

  sensors.setResolution(termometroInterno, TEMPERATURE_PRECISION);
  sensors.setResolution(termometroAgua, TEMPERATURE_PRECISION);
  sensors.setResolution(termometroAire, TEMPERATURE_PRECISION);
}

void loop()
{
  serialCom = digitalRead(serialComSwitch);

  if (serialCom)
  {
    SerialComMode();
  }
  else
  {
    dataGetMode();
  }
}

void SerialComMode()
{
  Serial.println(F("CS:OK"));

  while (serialCom)
  {
    int option = Serial.read();
    Serial.println(option);

    switch (option)
    {
    case '1':
      listDataFile();
      delay(1000);
      break;

    case '2':
      readDataFile();
      delay(1000);
      break;

    case '3':
      deleteDataFile();
      delay(1000);
      break;

    default:
      break;
    }

    serialCom = digitalRead(serialComSwitch);
  }
}

void dataGetMode()
{
  
  // Displays information if new sentence is correctly encoded.
  while (ss.available() > 0)
  {

    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid() && (gps.date.isValid() && gps.time.isValid()))
      {
        openDataFile();
        saveGpsData();
        saveTempData();
        savePressureData();
        saveUvData();
        dataFile.flush();
        closeDataFile();
        digitalWrite(endOP, HIGH);
        delay(60000);
      }
    }
  }

  // GPS error
  if (millis() > 20000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("Error: No se detecta el receptor GPS"));

    for (size_t i = 0; i < 10; i++)
    {
      digitalWrite(errorLED, HIGH);
      delay(1000);
      digitalWrite(errorLED, LOW);
      delay(1000);
    }
  }
}

void openDataFile()
{
  dataFile = SD.open("data.txt", FILE_WRITE);
  if (!dataFile)
  {
    Serial.println(F("Error: No se puede abrir el archivo de datos data.txt"));
    
    for (size_t i = 0; i < 3; i++)
    {
      digitalWrite(errorLED, HIGH);
      delay(3000);
      digitalWrite(errorLED, LOW);
      delay(1000);
    }
  }
}

void listDataFile()
{
  if (SD.exists(fileName))
  {
    // El archivo existe
    Serial.println(F("DF:EXISTE"));
  }

  else
  {
    // El archivo no existe
    Serial.println(F("DF:NO-EXISTE"));
  }

  Serial.println(F("CS:END"));
}

void readDataFile()
{
  File dataF = SD.open("data.txt");

  if (dataF)
  {
    while (dataF.available())
    {
      Serial.write(dataF.read());
    }
    dataF.flush();
    dataF.close();
  }

  else
  {
    // Error abriendo archivo
    Serial.println(F("DF:ERROR"));
  }

  Serial.println(F("CS:END"));
}

void deleteDataFile()
{
  if (SD.exists(fileName))
  {
    SD.remove(fileName);

    if (SD.exists(fileName))
    {
      // Error eliminando
      Serial.println(F("DF:ERROR"));
    }
    else
    {
      // Archivo eliminado correctamente
      Serial.println(F("DF:ELIMINADO"));
    }
  }

  else
  {
    // El archivo no existe
    Serial.println(F("DF:NO-EXISTE"));
  }

  Serial.println(F("CS:END"));
}

void closeDataFile()
{
  dataFile.println();
  dataFile.flush();
  dataFile.close();
}

void saveGpsData()
{
  // lat, lng, date(m/d/y), time(h:m:s:cs),

  dataFile.print(gps.location.lat(), 6);
  dataFile.print(F(","));
  dataFile.print(gps.location.lng(), 6);
  dataFile.print(F(","));
  dataFile.print(gps.date.year());
  dataFile.print(F("-"));
  dataFile.print(gps.date.month());
  dataFile.print(F("-"));
  dataFile.print(gps.date.day());
  dataFile.print(F(","));

  Serial.print(gps.location.lat(), 6);
  Serial.print(F(","));
  Serial.print(gps.location.lng(), 6);
  Serial.print(F(","));
  Serial.print(gps.date.year());
  Serial.print(F("-"));
  Serial.print(gps.date.month());
  Serial.print(F("-"));
  Serial.print(gps.date.day());
  Serial.print(F(","));

  if (gps.time.hour() < 10)
    dataFile.print(F("0"));
  
  dataFile.print(gps.time.hour());
  dataFile.print(F(":"));

    Serial.print(gps.time.hour());
    Serial.print(F(":"));

  if (gps.time.minute() < 10)
    dataFile.print(F("0"));
  
  dataFile.print(gps.time.minute());
  dataFile.print(F(":"));

    Serial.print(gps.time.minute());
    Serial.print(F(":"));

  if (gps.time.second() < 10)
    dataFile.print(F("0"));
  
  dataFile.print(gps.time.second());
  dataFile.print(F(","));

    Serial.print(gps.time.second());
    Serial.print(F(","));
}

void saveTempData()
{
  // tempInterna, tepmAgua, tempAire,
  sensors.requestTemperatures();
  delay(500);

  dataFile.print((sensors.getTempC(termometroInterno)), 4);
  dataFile.print(F(","));
  dataFile.print((sensors.getTempC(termometroAgua)), 4);
  dataFile.print(F(","));
  dataFile.print((sensors.getTempC(termometroAire)), 4);
  dataFile.print(F(","));

  // Serial
  Serial.print((sensors.getTempC(termometroInterno)), 4);
  Serial.print(F(","));
  Serial.print((sensors.getTempC(termometroAgua)), 4);
  Serial.print(F(","));
  Serial.print((sensors.getTempC(termometroAire)), 4);
  Serial.print(F(","));
}

void savePressureData()
{
  // pressure,

  float volt = readLinealAnalogSensorMv(0);

  if (volt < 0.2)
  {
    volt = 0.2;
  }
  
  float kPa = mapfloat(volt, 0.2, 4.5, 0.0, 703.125);

  dataFile.print(kPa, 4);
  dataFile.print(F(","));

  // Serial
  Serial.print(volt, 4);
  Serial.print(F(","));
}

void saveUvData()
{
  // UV

  int indice_uv = 0;
  float sum = 0;

  for (int i = 0; i < 50; i++)
  {
    int sensorValue = analogRead(A1);
    sum = sum + (sensorValue / 1024.0);
    delay(10);
  }

  float prom = sum / 50.0;
  
  if (prom * 1000 < 50) {
    indice_uv = 0;
  } else if (prom * 1000 < 227) {
    indice_uv = 1;
  } else if (prom * 1000 < 318) {
    indice_uv = 2;
  } else if (prom * 1000 < 408) {
    indice_uv = 3;
  } else if (prom * 1000 < 503) {
    indice_uv = 4;
  } else if (prom * 1000 < 606) {
    indice_uv = 5;
  } else if (prom * 1000 < 696) {
    indice_uv = 6;
  } else if (prom * 1000 < 795) {
    indice_uv = 7;
  } else if (prom * 1000 < 881) {
    indice_uv = 8;
  } else if (prom * 1000 < 976) {
    indice_uv = 9;
  } else if (prom * 1000 < 1079) {
    indice_uv = 10;
  } else {
    indice_uv = 11;
  }

  dataFile.print(indice_uv);

  Serial.println(indice_uv);
}

float readLinealAnalogSensorMv(int analogPin)
{
  float sum = 0;

  for (int i = 0; i < 50; i++)
  {
    float v = analogRead(analogPin);
    sum = sum + v / 1024.0;
    delay(10);
  }

  float sensorValue = sum / 50;
  float voltage = sensorValue * 5.0;
  
  return (voltage);
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}