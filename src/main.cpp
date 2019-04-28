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
#define serialComSwitch 4

// LED
#define errorLED 3

// SIGNAL
#define endOP 7

// GPS
static const int RXPin = 8, TXPin = 9;
SoftwareSerial ss(RXPin, TXPin);
TinyGPSPlus gps;

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

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
  sensors.begin();

  pinMode(chipSelect, OUTPUT);
  pinMode(serialComSwitch, INPUT);
  pinMode(errorLED, OUTPUT);
  pinMode(endOP, OUTPUT);

  Serial.println();
  Serial.println(F("termoDaQ V1.0"));
  Serial.println(F("[OSHW] VE000001"));
  Serial.println(F("https://github.com/mc-ireiser/termoDaQ"));
  Serial.println();

  // Test card
  if (!SD.begin(chipSelect))
  {
    Serial.println(F("Error: Verifique tarjeta SD"));
    // Wait forever
    while (1)
    {
      digitalWrite(errorLED, HIGH);
    }
  }
  Serial.println(F("Tarjeta SD inicializada"));

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
  Serial.println();
  Serial.println(F("Comunicacion serial establecida"));
  Serial.println(F("1) Comprobar existencia del DataFile"));
  Serial.println(F("2) Transmitir DataFile"));
  Serial.println(F("3) Eliminar DataFile"));

  while (serialCom)
  {
    int option = Serial.read();

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
      Serial.print(F("."));

      if (gps.location.isValid() && (gps.date.isValid() && gps.time.isValid()))
      {
        Serial.println();
        Serial.println();
        Serial.println(F("Data satelital recibida"));
        openDataFile();
        saveGpsData();
        saveTempData();
        savePressureData();
        saveUvData();
        Serial.println();
        dataFile.flush();
        closeDataFile();
        Serial.println(F("Ciclo de operacion terminado"));
        digitalWrite(endOP, HIGH);
        while (1)
          ;
      }
    }
  }

  // GPS error
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("Error: No se detecta el receptor GPS"));
    // Wait forever
    while (1)
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
    // Wait forever
    while (1)
    {
      digitalWrite(errorLED, HIGH);
      delay(3000);
      digitalWrite(errorLED, LOW);
      delay(1000);
    }
  }
  else
  {
    Serial.println(F("Archivo: data.txt - Abierto correctamente"));
    Serial.println();
  }
}

void listDataFile()
{
  Serial.println();

  if (SD.exists(fileName))
  {
    // El archivo existe
    Serial.println('+');
  }

  else
  {
    // El archivo no existe
    Serial.println('|');
  }
}

void readDataFile()
{
  Serial.println();

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
    Serial.println('|');
  }
}

void deleteDataFile()
{
  Serial.println();

  if (SD.exists(fileName))
  {
    SD.remove(fileName);

    if (SD.exists(fileName))
    {
      // Error eliminando
      Serial.println('|');
    }
    else
    {
      // Archivo eliminado correctamente
      Serial.println('+');
    }
  }

  else
  {
    // El archivo no existe
    Serial.println('|');
  }
}

void closeDataFile()
{
  dataFile.println();
  dataFile.flush();
  dataFile.close();
  Serial.println(F("Archivo: data.txt - Cerrado correctamente"));
}

void saveGpsData()
{
  // lat, lng, date(m/d/y), time(h:m:s:cs),
  Serial.println(F("--> GPS"));

  dataFile.print(gps.location.lat(), 6);
  dataFile.print(F(","));
  dataFile.print(gps.location.lng(), 6);
  dataFile.print(F(","));

  dataFile.print(gps.date.month());
  dataFile.print(F("/"));
  dataFile.print(gps.date.day());
  dataFile.print(F("/"));
  dataFile.print(gps.date.year());
  dataFile.print(F(","));

  if (gps.time.hour() < 10)
    dataFile.print(F("0"));
  dataFile.print(gps.time.hour());
  dataFile.print(F(":"));
  if (gps.time.minute() < 10)
    dataFile.print(F("0"));
  dataFile.print(gps.time.minute());
  dataFile.print(F(":"));
  if (gps.time.second() < 10)
    dataFile.print(F("0"));
  dataFile.print(gps.time.second());
  dataFile.print(F("."));
  if (gps.time.centisecond() < 10)
    dataFile.print(F("0"));
  dataFile.print(gps.time.centisecond());
  dataFile.print(F(","));

  Serial.println(F("    Data GPS almacenada"));
  Serial.println();
}

void saveTempData()
{
  // tempInterna, tepmAgua, tempAire,
  Serial.println(F("--> DS18B20"));

  Serial.print(F("    Termometros conectados: "));
  Serial.println(sensors.getDeviceCount(), DEC);

  sensors.requestTemperatures();
  delay(500);

  dataFile.print((sensors.getTempC(termometroInterno)), 4);
  dataFile.print(F(","));

  dataFile.print((sensors.getTempC(termometroAgua)), 4);
  dataFile.print(F(","));

  dataFile.print((sensors.getTempC(termometroAire)), 4);
  dataFile.print(F(","));

  Serial.println(F("    Data DS18B20 almacenada"));
  Serial.println();
}

void savePressureData()
{
  // pressure,
  Serial.println(F("--> MPX5700DP"));

  float volt = readLinealAnalogSensorMv(0) / 1024.0;
  dataFile.print(volt, 4);
  dataFile.print(F(","));

  Serial.println(F("    Data MPX5700DP almacenada"));
  Serial.println();
}

void saveUvData()
{
  // UV
  Serial.println(F("--> GUVA-S12SD"));

  float volt = readLinealAnalogSensorMv(3) / 1024.0;
  dataFile.print(volt, 4);

  Serial.println(F("    Data MPX5700DP almacenada"));
}

float readLinealAnalogSensorMv(int analogPin)
{
  float sum = 0;

  for (int i = 0; i < 1000; i++)
  {
    float v = analogRead(analogPin);
    sum = v + sum;
    delay(2);
  }

  return ((sum / 1000.0) * 5.0);
}
