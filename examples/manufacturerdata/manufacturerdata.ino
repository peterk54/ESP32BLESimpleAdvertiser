// manufacturerdata.ino
// klin, 31.03.2020
// advertise any manufacturer data
//
// based on lucascoelhof library esp32bleadvertise
// -> https://github.com/lucascoelhof/ESP32BleAdvertise

#include <ESP32BLESimpleAdvertiser.h>

// delay time in seconds
const int delaytime = 5;

// advertiser object
BLESimpleAdvertiser bleadv;

// name of ble advertiser device - for better detecting in payload
const char *blename = "BLE-MMM";

// setup
void setup() 
{
  Serial.begin(115200);
  // init ble advertiser
  bleadv.begin(blename); 
  Serial.printf("# start manufacturer data advertising - device %s - ", blename);
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
  // build advertising manufacturer data string
  sprintf(data, "#%d#", ++count);
  int len = strlen(data);
  Serial.printf("> advertise [%s] ", data);
  for(int i = 0; i < len; i++)
    Serial.printf("%02x ", data[i]);
  Serial.printf("\n"); 
  // advertise the manufacturer data
  bleadv.manufacturerDataAdvertise(data);
  delay(delaytime * 1000);
}
