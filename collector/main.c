#include <string.h>
#include <timer.h>

#include "uartadv.h"
#include "rf233-const.h"
#include "rf233-config.h"
#include "rf233-arch.h"
#include "trx_access.h"
#include "rf233.h"

#define SQUALL_COUNT 10

typedef struct Squall {
 int rssi;
} Squall;

static Squall squalls[SQUALL_COUNT];

/*
 * Sends the current status to the receiver
 * Sends the ID of the closest squall (one byte) and its RSSI (two bytes)
 */
void send_to_receiver() {
  int rssi[SQUALL_COUNT];

  for (int i = 0; i < SQUALL_COUNT; i++){
    Squall *sq = &squalls[i];
    rssi[i] = sq->rssi;
    sq->rssi = 0;
  }

  rf233_tx_data(0x00, (uint8_t*)rssi, sizeof(int) * SQUALL_COUNT);
  for (int i = 0; i < SQUALL_COUNT; i++) {
    printf("%d\t", rssi[i]);
  }
  printf("\n");

  delay_ms(10);
}

void adv_callback(int r0, int r1, int r3, void* ud) {
  uint8_t *buf = (uint8_t*)ud;
  if (buf[0] == 0) {
    // Adv packet
    // buf[1-6] address
    // buf[7] rssi
    // buf[8] advertising payload length
    // buf[9-...] payload
    
    if (buf[6] == 0xC0 &&
        buf[5] == 0x98 &&
        buf[4] == 0xE5) {
      led_toggle(0);

      uint8_t id = buf[1];
      int8_t rssi = (int8_t)buf[7];

      if (squalls[id % SQUALL_COUNT].rssi > rssi) {
        squalls[id % SQUALL_COUNT].rssi = rssi;
      }
    }
  }
  ble_advs_start();
}

char adv_buf[64]; 

int main () {
    rf233_init(0xab, 0xbc, 0xcd);
    ble_advs_allow(adv_buf, sizeof(adv_buf));
    ble_advs_subscribe(adv_callback, adv_buf);
    ble_advs_start();

    while(1) {
      delay_ms(10000);
      send_to_receiver();
    }
}

