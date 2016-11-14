#include <string.h>
#include <timer.h>

#include "rf233-const.h"
#include "rf233-config.h"
#include "rf233-arch.h"
#include "trx_access.h"
#include "rf233.h"

#define MAX_SQUALL_COUNT 10

typedef struct Squall {
 char id;
 int rsi;
} Squall;

static Squall squalls[MAX_SQUALL_COUNT];

/*
 * Sends the current status to the receiver
 * Sends the ID of the closest squall (one byte) and its RSI (two bytes)
 */
void send_to_receiver() {
  char id;
  int rsi = 0;

  for (int i = 0; i < MAX_SQUALL_COUNT; i++){
    Squall sq = squalls[i];

    if (sq.rsi > rsi){
      id = sq.id;
      rsi = sq.rsi;
    }
  }

  char buf[3] = { id, (char)(rsi & 0x11110000), (char)(rsi & 0x00001111) };

  rf233_tx_data(0x00, buf, 3);
  delay_ms(10);
}

void adv_callback(int r0, int r1, int r3, void* ud) {
  uint8_t *buf = (uint8_t*)ud;
  printf("Got data\n");
  if (buf[0] == 0) {
    // Adv packet
  }
}

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

