// ESP32BLESimpleAdvertiser.h
// klin, 31.03.2020
//
// thanks to lucas coelho figueiredo 
// -> https://github.com/lucascoelhof/ESP32BleAdvertise

// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _BLE_SIMPLE_ADVERTISER_H_
#define _BLE_SIMPLE_ADVERTISER_H_

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bt.h"

#include "Arduino.h"
#include "BLEUUID.h"

//struct ble_gap_adv_params_s;

class BLESimpleAdvertiser {
  
  public:

    BLESimpleAdvertiser(void);
    ~BLESimpleAdvertiser(void);

    bool begin(String localName = String());

    bool manufacturerDataAdvertise(String data);
    bool manufacturerDataAdvertise(byte *data, int size);

    bool serviceDataAdvertise(String data);
    bool serviceDataAdvertise(uint16_t uuid, String data); 
    bool serviceDataAdvertise(char *uuid, String data);
    bool serviceDataAdvertise(String uuid, String data);
    bool serviceDataAdvertise(BLEUUID uuid, String data);
       
    bool serviceDataAdvertise(byte *data, int size);
    bool serviceDataAdvertise(uint16_t uuid, byte *data, int size); 
    bool serviceDataAdvertise(char *uuid, byte *data, int size);
    bool serviceDataAdvertise(String uuid, byte *data, int size);  
    bool serviceDataAdvertise(BLEUUID uuid, byte *data, int size);
    
    void end(void);

  private:
        
    String local_name;
    void clearAdvertiseData();
    void fillManufacturerData(byte *data, int size);
    void fillServiceData(byte *data, int size);
    void fillServiceData(uint16_t uuid, byte *data, int size);  // klin

};

#endif
