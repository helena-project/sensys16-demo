#pragma once

#include <tock.h>

#define BLE_ADV_DRV_NUM 0xbe

int ble_advs_allow(char* buffer, int len);

int ble_advs_subscribe(subscribe_cb cb, void *userdata);

int ble_advs_start();

int ble_advs_stop();
