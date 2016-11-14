#include <tock.h>
#include "uartadv.h"

int ble_advs_allow(char* buffer, int len) {
  return allow(BLE_ADV_DRV_NUM, 0, buffer, len);
}

int ble_advs_subscribe(subscribe_cb cb, void *userdata) {
  return subscribe(BLE_ADV_DRV_NUM, 0, cb, userdata);
}

int ble_advs_start() {
  return command(BLE_ADV_DRV_NUM, 0, 0);
}

int ble_advs_stop() {
  return command(BLE_ADV_DRV_NUM, 1, 0);
}
