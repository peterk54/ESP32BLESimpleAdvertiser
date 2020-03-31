// receiver.ino
// klin, 31.03.2020
// scan ble devices advertised manufacturer and servie data
//
// modified scanner example from esp32 ble library
// based on work of n. kolban and e. copercino

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEUUID.h>

// ble scanner object
BLEScan *scanner; 

// scan time in seconds
const int scantime = 5;

// delaytime in seconds
const int delaytime = 1;
                      
// print payload data
void printpayload(uint8_t *payload, int length)
{
  Serial.printf("> payload %2d bytes:  ", length);
  for(int i = 0; i < length; i++)
    Serial.printf("%02x ", payload[i]);
  Serial.printf("\n");   
}

// advertising callbacks from scanner
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  // advertising device found
  void onResult(BLEAdvertisedDevice advdev) {
    // manufacturer data advertising device found
    if(advdev.haveManufacturerData()) {      
      Serial.printf("> manufacturer data: name:[%s] addr:[%s] rssi:%d data:[%s]\n", 
                    advdev.getName().c_str(), 
                    advdev.getAddress().toString().c_str(),
                    advdev.getRSSI(), 
                    advdev.getManufacturerData().c_str());                    
      printpayload(advdev.getPayload(), advdev.getPayloadLength());              
    }
    // service data advertising device found
    if(advdev.haveServiceData()) {
      Serial.printf("> service data:      name:[%s] addr:[%s] rssi:%d uuid:[%s] data:[%s]\n", 
                    advdev.getName().c_str(), 
                    advdev.getAddress().toString().c_str(),
                    advdev.getRSSI(), 
                    advdev.getServiceDataUUID().toString().c_str(), 
                    advdev.getServiceData().c_str());
      printpayload(advdev.getPayload(), advdev.getPayloadLength()); 
    }
  }
};

// setup
void setup() 
{
  Serial.begin(115200);
  Serial.println("# esp32 ble advertisement receiver");
  // init ble device (without ble name)
  BLEDevice::init("");
  // create and init scanner
  scanner = BLEDevice::getScan();
  scanner->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  scanner->setActiveScan(true); 
  scanner->setInterval(100);
  scanner->setWindow(99); 
}

// loop
void loop() 
{
  Serial.println("# scanning ...");
  // start scanner
  BLEScanResults devices = scanner->start(scantime, false);
  // free memory after scan is done
  scanner->clearResults();  
  Serial.println("# ... scan done");
  delay(delaytime * 1000);
}