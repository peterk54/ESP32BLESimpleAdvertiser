// servicedata.ino
// klin, 31.03.2020
// advertise any service data
//
// based on lucascoelhof library esp32bleadvertise
// -> https://github.com/lucascoelhof/ESP32BleAdvertise

#include <ESP32BLESimpleAdvertiser.h>
#include <BLEUUID.h>

// advertiser service data uuid (16 bit uuid 0xAABB for better detecting in payload)
//uint16_t uuid = 0xAABB;
//char *uuid    = "AABB";
//String uuid   = "AABB";
BLEUUID uuid = BLEUUID(uint16_t(0xAABB));

// delay time in seconds
const int delaytime = 5;

// advertiser object
BLESimpleAdvertiser bleadv;

// name of ble advertiser device - for better detecting in payload
const char *blename = "BLE-SSS";

// setup
void setup() 
{
  Serial.begin(115200);
  // init ble advertiser
  bleadv.begin(blename); 
  Serial.printf("# start service data advertising - device %s - ", blename);
  int len = strlen(blename);
  for(int i = 0; i < len; i++)
    Serial.printf("%02x ", blename[i]);
  Serial.printf("\n");
}

// loop
void loop() 
{
  static int count = 0;
  char data[32];
  // build advertising service data string
  sprintf(data, "#%d#", ++count);
  int len = strlen(data);
  Serial.printf("> advertise [%s] ", data);
  for(int i = 0; i < len; i++)
    Serial.printf("%02x ", data[i]);
  Serial.printf("\n"); 
  // advertise the service uuid and data
  bleadv.serviceDataAdvertise(uuid, data);
  delay(delaytime * 1000);
}
