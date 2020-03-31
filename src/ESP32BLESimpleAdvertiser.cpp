// ESP32BLESimpleAdvertiser.cpp
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

#include "ESP32BLESimpleAdvertiser.h"
#include "esp32-hal-log.h"

#include "bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#define MAX_MANUFACTURER_DATA_SIZE 20
#define MAX_SERVICE_DATA_SIZE 11

// data will be advertised
static esp_ble_adv_data_t adv_data; 

// advertising data configuration
static esp_ble_adv_data_t _adv_config = {
  .set_scan_rsp         = false,
  .include_name         = true,
  .include_txpower      = true,
  .min_interval         = 512,
  .max_interval         = 1024,
  .appearance           = 0,
  .manufacturer_len     = 0,
  .p_manufacturer_data  = NULL,
  .service_data_len     = 0,
  .p_service_data       = NULL,
  .service_uuid_len     = 0,
  .p_service_uuid       = NULL,
  .flag                 = (ESP_BLE_ADV_FLAG_NON_LIMIT_DISC|ESP_BLE_ADV_FLAG_BREDR_NOT_SPT)
};

// advertising data parameter
static esp_ble_adv_params_t _adv_params = {
  .adv_int_min          = 512,
  .adv_int_max          = 1024,
  .adv_type             = ADV_TYPE_NONCONN_IND,  // excellent description of this parameter here: https://www.esp32.com/viewtopic.php?t=2267
  .own_addr_type        = BLE_ADDR_TYPE_PUBLIC,         
  .peer_addr            = { 0x00, },  
  .peer_addr_type       = BLE_ADDR_TYPE_PUBLIC,
  .channel_map          = ADV_CHNL_ALL,
  .adv_filter_policy    = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// internal buffer
static byte dataBuffer[64];

// ble gap functions

// gap callback handler
static void _on_gap(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
  if(event == ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT) {
    esp_ble_gap_start_advertising(&_adv_params);
  }
}

// initialize gap
static bool _init_gap(const char *name, esp_ble_adv_data_t *adv_data){
  if( !btStarted() && !btStart()) {
    log_e("btStart failed");
    return false;
  }
  esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
  if(bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED) {
    if(esp_bluedroid_init()) {
      log_e("esp_bluedroid_init failed");
      return false;
    }
  }
  if(bt_state != ESP_BLUEDROID_STATUS_ENABLED) {
    if(esp_bluedroid_enable()) {
      log_e("esp_bluedroid_enable failed");
      return false;
    }
  }
  if(esp_ble_gap_set_device_name(name)) {
    log_e("gap_set_device_name failed");
    return false;
  }
  if(esp_ble_gap_config_adv_data(adv_data)) {
    log_e("gap_config_adv_data failed");
    return false;
  }
  if(esp_ble_gap_register_callback(_on_gap)) {
    log_e("gap_register_callback failed");
    return false;
  }
  return true;
}

// stop gap
static bool _stop_gap()
{
  if(btStarted()) {
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    btStop();
  }
  return true;
}

// BLESimpleAdvertiser class

// constructor
BLESimpleAdvertiser::BLESimpleAdvertiser()
{
  local_name = "esp32";
  adv_data = {
    .set_scan_rsp        = false,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 512,
    .max_interval        = 1024,
    .appearance          = 0,
    .manufacturer_len    = 0,
    .p_manufacturer_data = NULL,  // manufacturer data ...
    .service_data_len    = 0,
    .p_service_data      = NULL,  // ... or service data will be advertised
    .service_uuid_len    = 0,
    .p_service_uuid      = NULL,
    .flag                = (ESP_BLE_ADV_FLAG_NON_LIMIT_DISC)
  };
}

// destructor
BLESimpleAdvertiser::~BLESimpleAdvertiser(void)
{
  clearAdvertiseData();
  _stop_gap();
}

// begin method
bool BLESimpleAdvertiser::begin(String localName)
{
  if(localName.length()) {
    local_name = localName;
  }
  return _init_gap(local_name.c_str(), &_adv_config);
}

// end method
void BLESimpleAdvertiser::end()
{
  _stop_gap();
}

// manufacturer data advertise string
bool BLESimpleAdvertiser::manufacturerDataAdvertise(String data) 
{
  data.getBytes(dataBuffer, data.length() + 1);
  return manufacturerDataAdvertise(dataBuffer, data.length());
}

// manufacturer data advertise bytes
bool BLESimpleAdvertiser::manufacturerDataAdvertise(byte *data, int size) 
{
  clearAdvertiseData();
  fillManufacturerData(data, size);
  return _init_gap(local_name.c_str(), &adv_data);
}

// service data advertise string
bool BLESimpleAdvertiser::serviceDataAdvertise(String data) 
{
  data.getBytes(dataBuffer, data.length() + 1);
  return serviceDataAdvertise(dataBuffer, data.length());
}

// service data advertise string with service data uuid
bool BLESimpleAdvertiser::serviceDataAdvertise(uint16_t uuid, String data) 
{
  data.getBytes(dataBuffer, data.length() + 1);
  return serviceDataAdvertise(uuid, dataBuffer, data.length());
}

// service data advertise string with service data uuid as char * parameter
bool BLESimpleAdvertiser::serviceDataAdvertise(char *uuid, String data) 
{
  uint16_t u16 = (uint16_t) strtol(uuid, NULL, 16);
  data.getBytes(dataBuffer, data.length() + 1);
  return serviceDataAdvertise(u16, dataBuffer, data.length());
}

// service data advertise string with service data uuid as string parameter
bool BLESimpleAdvertiser::serviceDataAdvertise(String uuid, String data) 
{
  uint16_t u16 = (uint16_t) strtol(uuid.c_str(), 0, 16);
  data.getBytes(dataBuffer, data.length() + 1);
  return serviceDataAdvertise(u16, dataBuffer, data.length());
}

// service data advertise string with service data uuid as bleuuid parameter
bool BLESimpleAdvertiser::serviceDataAdvertise(BLEUUID uuid, String data) 
{
  uint16_t u16 = (uint16_t) strtol(uuid.toString().c_str(), 0, 16);
  data.getBytes(dataBuffer, data.length() + 1);
  return serviceDataAdvertise(u16, dataBuffer, data.length());
}

// service data advertise bytes
bool BLESimpleAdvertiser::serviceDataAdvertise(byte *data, int size) 
{
  clearAdvertiseData();
  fillServiceData(data, size);
  return _init_gap(local_name.c_str(), &adv_data);
}

// service data advertise bytes with service data uuid
bool BLESimpleAdvertiser::serviceDataAdvertise(uint16_t uuid, byte *data, int size) 
{
  clearAdvertiseData();
  fillServiceData(uuid, data, size);
  return _init_gap(local_name.c_str(), &adv_data);
}

// service data advertise bytes with service data uuid as char * parameter
bool BLESimpleAdvertiser::serviceDataAdvertise(char *uuid, byte *data, int size) 
{
  uint16_t u16 = (uint16_t) strtol(uuid, NULL, 16);
  clearAdvertiseData();
  fillServiceData(u16, data, size);
  return _init_gap(local_name.c_str(), &adv_data);
}

// service data advertise bytes with service data uuid as string parameter
bool BLESimpleAdvertiser::serviceDataAdvertise(String uuid, byte *data, int size) 
{
  uint16_t u16 = (uint16_t) strtol(uuid.c_str(), 0, 16);
  clearAdvertiseData();
  fillServiceData(u16, data, size);
  return _init_gap(local_name.c_str(), &adv_data);
}

// service data advertise bytes with service data uuid as bleuuid parameter
bool BLESimpleAdvertiser::serviceDataAdvertise(BLEUUID uuid, byte *data, int size) 
{
  uint16_t u16 = (uint16_t) strtol(uuid.toString().c_str(), 0, 16);
  clearAdvertiseData();
  fillServiceData(u16, data, size);
  return _init_gap(local_name.c_str(), &adv_data);
}

// clear allocated data memory
void BLESimpleAdvertiser::clearAdvertiseData() 
{
  if(adv_data.p_manufacturer_data != NULL) {
    free(adv_data.p_manufacturer_data);
    adv_data.p_manufacturer_data = NULL;
    adv_data.manufacturer_len = 0;
  }
  if(adv_data.p_service_data != NULL) {
    free(adv_data.p_service_data);
    adv_data.p_service_data = NULL;
    adv_data.service_data_len = 0;
  }
}

// fill manufacturer data 
void BLESimpleAdvertiser::fillManufacturerData(byte *data, int size) 
{
  if(size > MAX_MANUFACTURER_DATA_SIZE)
    size = MAX_MANUFACTURER_DATA_SIZE;
  adv_data.p_manufacturer_data = (uint8_t *) malloc(size * sizeof(uint8_t));
  adv_data.manufacturer_len = size;
  memcpy(adv_data.p_manufacturer_data, data, size);
}

// fill service data
void BLESimpleAdvertiser::fillServiceData(byte *data, int size) 
{
  if(size > MAX_SERVICE_DATA_SIZE)
    size = MAX_SERVICE_DATA_SIZE;
  adv_data.p_service_data = (uint8_t *) malloc(size * sizeof(uint8_t));
  adv_data.service_data_len = size;
  memcpy(adv_data.p_service_data, data, size);
}

// fill service data at given service data uuid
void BLESimpleAdvertiser::fillServiceData(uint16_t uuid, byte *data, int size)
{ 
  if(size > (MAX_SERVICE_DATA_SIZE - 2))  // 2 bytes needed for uuid
    size = MAX_SERVICE_DATA_SIZE - 2;
  adv_data.p_service_data = (uint8_t *) malloc((size + 2) * sizeof(uint8_t));
  adv_data.service_data_len = size + 2;
  adv_data.p_service_data[0] =  uuid & 0xff;
  adv_data.p_service_data[1] = (uuid >> 8) & 0xff; 
  memcpy(&adv_data.p_service_data[2], data, size);
}
