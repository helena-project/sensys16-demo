#include <string.h>
#include <ble_advdata.h>

#include <simple_ble.h>
#include <simple_adv.h>
#include <eddystone.h>
#include <nrf51_serialization.h>
#include <timer.h>

#include "room_location_service.h"


/*******************************************************************************
 * BLE
 ******************************************************************************/

// Intervals for advertising and connections
//char device_name[] = "FSTORM";
simple_ble_config_t ble_config = {
    .platform_id       = 0x00,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = "FSTORM",
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1250, UNIT_1_25_MS)
};

// URL to advertise
char eddystone_url[] = "goo.gl/8685Uw";

// Manufacturer specific data setup
#define UMICH_COMPANY_IDENTIFIER 0x02E0
#define BLE_APP_ID  0x16
#define BLE_APP_VERSION_NUM 0x00
uint8_t mdata[4] = {BLE_APP_ID, BLE_APP_VERSION_NUM, 0xFF, 0xFF};
ble_advdata_manuf_data_t mandata = {
  .company_identifier = UMICH_COMPANY_IDENTIFIER,
  .data.p_data = mdata,
  .data.size   = sizeof(mdata)
};

void ble_address_set () {
  // nop
}

void ble_error (uint32_t error_code) {
    printf("BLE ERROR: Code = %d\n", (int)error_code);
}

void services_init (void) {
  room_location_service_init();
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/


static uint32_t next_rand(void) {

  static uint32_t x = 13;
  static uint32_t y = 99;
  static uint32_t z = 32;
  static uint32_t w = 1;

  uint32_t t = x;
  t ^= t << 11;
  t ^= t >> 8;
  x = y; y = z; z = w;
  w ^= w >> 19;
  w ^= t;
  return w;
}

int main () {
    printf("Starting BLE serialization example\n");

    // Setup BLE
    simple_ble_init(&ble_config);

    ble_advdata_t srdata;
    memset(&srdata, 0, sizeof(srdata));

    srdata.name_type = BLE_ADVDATA_FULL_NAME;
    srdata.p_manuf_specific_data = &mandata;
    ble_uuid_t PHYSWEB_SERVICE_UUID[] = {{0x181A, BLE_UUID_TYPE_BLE}};
    ble_advdata_uuid_list_t service_list = {
      .uuid_cnt = 1,
      .p_uuids = PHYSWEB_SERVICE_UUID
    };
    srdata.uuids_complete = service_list;

    // And update advertising data
    eddystone_adv(eddystone_url, &srdata);

    while(1) {
      delay_ms(500);
      for (int i = 0; i < 1; i++) {
        int8_t loc = (int8_t)next_rand();
        room_location_update_item(i, loc);
      }
    }
}

