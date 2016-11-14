#pragma once

typedef uint16_t uuid_t;

enum SPI_OPCODES {
  BLE_ADVERTISE = 0, // advertisement received
  // addition opcodes go here
  BLE_DEBUG = 0xff
};
