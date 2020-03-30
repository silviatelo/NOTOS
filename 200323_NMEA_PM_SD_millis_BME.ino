// Proyecto NOTOS - Medición de partículas volátiles (PM10, PM2.5)
// Mediación-Investigación 2018-2020
// Marzo 2020 - Silvia Teixeira en Medialab-Prado

#include <NMEAGPS.h>
#include <GPSport.h>
#include <Streamers.h>

#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h> 

#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

Adafruit_BME680 bme; // I2C

#define SEALEVELPRESSURE_HPA (1013.25)

#define PIN 49

#define pmsSerial Serial1

unsigned long interval = 10000;
unsigned long previousMillis = 0;
unsigned long previousMillisGPS = 0;

static NMEAGPS  gps;
static gps_fix  fix;
static void doSomeWork()
{
  trace_all( DEBUG_PORT, gps, fix );
} 

static void GPSloop()
{
  while (gps.available( gpsPort )) 
  {
    unsigned long currentMillisGPS = millis();
    
    if (currentMillisGPS - previousMillisGPS > interval){
      fix = gps.read();
      Serial.print("Fecha:");
      Serial.print(fix.dateTime.year);
      Serial.print('/');
      Serial.print(fix.dateTime.month);
      Serial.print('/');
      Serial.print(fix.dateTime.date);
      Serial.println();
      Serial.print("Hora (UTC):");
      Serial.print(fix.dateTime.hours);
      Serial.print(':');
      Serial.print(fix.dateTime.minutes);
      Serial.println();
      Serial.print("Latitud,Longitud:");
      Serial.print(fix.latitudeL());
      Serial.print(',');
      Serial.print(fix.longitudeL());
      Serial.println();
      Serial.print("Altitud (float m):");
      Serial.println(fix.altitude());
      Serial.print("Satélites:");
      Serial.println(fix.satellites);
      
      doSomeWork();
      Serial.println();
      previousMillisGPS = millis();
    }
  }
} 

const int ChipSelect=10;

#define LOG_FILE_PREFIX "PMlog" // Name of the log file.
#define MAX_LOG_FILES 100 // Number of log files that can be made
#define LOG_FILE_SUFFIX "csv" // Suffix of the log file
char logFileName[13]; // Char string to store the log file name
// Data to be logged:
#define LOG_COLUMN_COUNT 17

char * log_col_names[LOG_COLUMN_COUNT] = {
  "1um (ug/m3)", "2.5um (ug/m3)", "10um (ug/m3)","1um", "2.5um", "10um", "Fecha", "Hora", "Latitud", "Longitud","Altitud (GPS)", "Satelites", "Temp", "Presion", "Humedad", "Gas", "Altitud (Bar)"
}; // log_col_names is printed at the top of the file.

File logFile;


//uint32_t getAbsoluteHumidity(float temperature, float humidity) {
//    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
//    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
//    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
//    return absoluteHumidityScaled;
//}

void setup()
{
  DEBUG_PORT.begin(9600);
  gpsPort.begin( 9600 );

  Serial.begin(9600); 
  pmsSerial.begin(9600);

  // SD Card Initialization
  if (!SD.begin(ChipSelect))
  {
    Serial.println("Error iniciando la tarjeta SD.");
    return;
  }
  Serial.println("Tarjeta SD iniciada.");


  updateFileName();
  printHeader();


 //open file
  logFile=SD.open("nombreArchivo", FILE_WRITE);

  // if the file opened ok, write to it:
  if (logFile) 
  {
    Serial.println("Open File");
    // print the headings for our data
    logFile.println("Particle Concentration");
  }
  
  logFile.close();

}

struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};

struct pms5003data data;

void loop()
{    
  unsigned long currentMillis = millis();
  
  GPSloop();

  if (currentMillis - previousMillis > interval){
    if (readPMSdata(&pmsSerial)) 
    {
      // reading data was successful!
      Serial.println();
      Serial.println("---------------------------------------");
      Serial.println("Unidades de Concentration (ug/m3)");
      Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
      Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
      Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
      Serial.println("---------------------------------------");
      Serial.print("Particulas > 0.3um / 0.1L aire:"); Serial.println(data.particles_03um);
      Serial.print("Particulas > 2.5um / 0.1L aire:"); Serial.println(data.particles_25um);
      Serial.print("Particulas > 10.0 um / 0.1L aire:"); Serial.println(data.particles_100um);
      Serial.println("---------------------------------------");

      if (bme.begin())
      {
          Serial.print("Temperatura = "); Serial.print(bme.temperature); Serial.println(" *C");
          Serial.print("Presion = "); Serial.print(bme.pressure / 100.0); Serial.println(" hPa");
          Serial.print("Humedad = "); Serial.print(bme.humidity); Serial.println(" %");
          Serial.print("Gas = "); Serial.print(bme.gas_resistance / 1000.0); Serial.println(" KOhms");
          Serial.print("Altitud = "); Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA)); Serial.println(" m");
          Serial.println();
      }

      GuardarPM();  

      previousMillis = millis();
    }
  }
}

byte GuardarPM()
{
    logFile = SD.open(logFileName, FILE_WRITE);
    if(logFile)
    {
      logFile.print(data.pm10_env);
      logFile.print(',');
      logFile.print(data.pm25_env);
      logFile.print(',');
      logFile.print(data.pm100_env);
      logFile.print(',');
      logFile.print(data.particles_03um);
      logFile.print(',');
      logFile.print(data.particles_25um);
      logFile.print(',');
      logFile.print(data.particles_100um);
      logFile.print(',');
      logFile.print(fix.dateTime.date);
      logFile.print('/');
      logFile.print(fix.dateTime.month);
      logFile.print('/');
      logFile.print(fix.dateTime.year);
      logFile.print(',');
      logFile.print(fix.dateTime.hours);
      logFile.print(':');
      logFile.print(fix.dateTime.minutes);
      logFile.print(',');
      logFile.print(fix.latitudeL());
      logFile.print(',');
      logFile.print(fix.longitudeL());
      logFile.print(',');
      logFile.print(fix.altitude());
      logFile.print(',');
      logFile.print(fix.satellites);
      logFile.print(',');
      logFile.print(bme.temperature);
      logFile.print(',');
      logFile.print(bme.pressure / 100.0);
      logFile.print(',');
      logFile.print(bme.humidity);
      logFile.print(',');
      logFile.print(bme.gas_resistance / 1000.0);
      logFile.print(',');
      logFile.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
      logFile.println();
      

      logFile.close();
      return 1;
    }
    return 0;
 }
  
void printHeader()
{
  logFile = SD.open(logFileName, FILE_WRITE); // Open the log file

  if (logFile) // If the log file opened, print our column names to the file
  {
    for (int i=0; i < LOG_COLUMN_COUNT; i++)
    {
      logFile.print(log_col_names[i]);
      if (i < LOG_COLUMN_COUNT - 1) // If it's anything but the last column
        logFile.print(','); // print a comma
      else // If it's the last column
        logFile.println(); // print a new line
    }
    logFile.close(); // close the file
  }
}

// updateFileName() - Looks through the log files already present on a card,
// and creates a new file with an incremented file index.
void updateFileName()
{
  int i = 0;
  for (; i < MAX_LOG_FILES; i++)
  {
    memset(logFileName, 0, strlen(logFileName)); // Clear logFileName string
    // Set logFileName to "PMlogXX.csv":
    sprintf(logFileName, "%s%d.%s", LOG_FILE_PREFIX, i, LOG_FILE_SUFFIX);
    if (!SD.exists(logFileName)) // If a file doesn't exist
    {
      break; // Break out of this loop. We found our index
    }
    else // Otherwise:
    {
      Serial.print(logFileName);
      Serial.println(" exists"); // Print a debug statement
    }
  }
  Serial.print("File name: ");
  Serial.println(logFileName); // Debug print the file name
}


boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }

  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);

  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}


