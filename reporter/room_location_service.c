#include <stdint.h>
#include <string.h>

#include "simple_ble.h"
#include "app_error.h"
#include "ble.h"
#include "room_location_service.h"

static simple_ble_service_t room_location_service = {
    //.uuid128 = {{0x4a, 0xb6, 0xbe, 0x69, 0x8c, 0xa8, 0x40, 0x7a,
    //             0x95, 0x25, 0xef, 0xe6, 0x76, 0x93, 0xb0, 0xfa}}
    .uuid128 = {{0x87, 0xa4, 0xde, 0xa0, 0x96, 0xea, 0x4e, 0xe6,
                 0x87, 0x45, 0x83, 0x28, 0x89, 0x0f, 0xad, 0x7b}}
};

static simple_ble_char_t item_chars[5] = {
  {.uuid16 = 0x8910},
  {.uuid16 = 0x8911},
  {.uuid16 = 0x8912},
  {.uuid16 = 0x8913},
  {.uuid16 = 0x8914},
};

static int8_t item_location_ids[5] = {-1, -1, -1, -1, -1};

static void add_item_char(int i) {
  uint32_t err_code;
  ble_gatts_char_md_t char_md;
  ble_gatts_attr_t    attr_char_value;
  ble_uuid_t          char_uuid;
  ble_gatts_attr_md_t attr_md;

  // set characteristic metadata
  memset(&char_md, 0, sizeof(char_md));
  char_md.char_props.read   = false;
  char_md.char_props.notify = true;

  // set characteristic uuid
  char_uuid.type = BLE_UUID_TYPE_BLE;
  char_uuid.uuid = 0x8910 + i;

  // set attribute metadata
  memset(&attr_md, 0, sizeof(attr_md));
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
  BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
  attr_md.vloc    = BLE_GATTS_VLOC_STACK;

  // set attribute data
  memset(&attr_char_value, 0, sizeof(attr_char_value));
  attr_char_value.p_uuid    = &char_uuid;
  attr_char_value.p_attr_md = &attr_md;
  attr_char_value.init_len  = 1;
  attr_char_value.init_offs = 0;
  attr_char_value.max_len   = 1;
  attr_char_value.p_value   = (uint8_t*)&item_location_ids[i];

  err_code = sd_ble_gatts_characteristic_add(room_location_service.service_handle,
          &char_md, &attr_char_value, &(item_chars[i].char_handle));
  APP_ERROR_CHECK(err_code);
}

void room_location_service_init() {

  // add service
  simple_ble_add_service(&room_location_service);


  // add characteristics
  for (int i = 0; i < 5; i++) {
    add_item_char(i);
  }
}

uint32_t room_location_update_item(int item, int8_t location_id) {
  item_location_ids[item] = location_id;
  ble_gatts_value_t value = {
      .len = 1,
      .offset = 0,
      .p_value = (uint8_t*)&item_location_ids[item],
  };
  int err_code = sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID,
              item_chars[item].char_handle.value_handle, &value);
  if (err_code != NRF_SUCCESS) {
    return err_code;
  }

  return simple_ble_notify_char(&item_chars[item]);
}

