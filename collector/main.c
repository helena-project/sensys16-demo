#include <string.h>
#include <timer.h>

#include "uartadv.h"
#include "rf233-const.h"
#include "rf233-config.h"
#include "rf233-arch.h"
#include "trx_access.h"
#include "rf233.h"

#define MAX_SQUALL_COUNT 10
#define ADDRESS_SIZE 6

typedef struct Squall {
 uint8_t address[ADDRESS_SIZE];
 int rssi;
} Squall;

static Squall squalls[MAX_SQUALL_COUNT];
static int squall_count = 0;

/*
 * Sends the current status to the receiver
 * Sends the ID of the closest squall (one byte) and its RSSI (two bytes)
 */
void send_to_receiver() {
  int rssi = 0;
  int sq_index = 0;

  for (int i = 0; i < MAX_SQUALL_COUNT; i++){
    Squall sq = squalls[i];

    if (sq.rssi > rssi){
      sq_index = i;
      rssi = sq.rssi;
    }
  }

  uint8_t buf[ADDRESS_SIZE + 1];
  for (int i = 0; i < ADDRESS_SIZE; i++){
    buf[i] = squalls[sq_index].address[i];
  }
  buf[ADDRESS_SIZE] = rssi;

  rf233_tx_data(0x00, buf, ADDRESS_SIZE + 1);
  delay_ms(10);
}

/*
 * Goes through an address buffer and checks if every element equals the address of the squall
 */
static int addresses_match(Squall sq, uint8_t *buf){
    for (int i = 0; i < ADDRESS_SIZE; i++){
      if (sq.address[i] != buf[i]){
        return 0;
      }
    }

    return 1;
}

void adv_callback(int r0, int r1, int r3, void* ud) {
  uint8_t *buf = (uint8_t*)ud;
  printf("Got data\n");
  if (buf[0] == 0) {
    // Adv packet
    // buf[1-6] address
    // buf[7] rssi
    // buf[8] advertising payload length
    // buf[9-...] payload

    uint8_t address[ADDRESS_SIZE];
    for (int i = 0; i < ADDRESS_SIZE; i++){
        address[i] = buf[i + 1];
    }
    uint8_t rssi = buf[7];

    //find the right squall
    int sq_index = -1;
    for (int i = 0; i < MAX_SQUALL_COUNT; i++){
      if (addresses_match(squalls[i], address)){
        sq_index = i;
        break;
      }
    }

    //if it didn't find the right squall, make it create a new one in the next slot
    if (sq_index == -1){
        sq_index = squall_count;
        squall_count ++;
    }

    for (int i = 0; i < ADDRESS_SIZE; i++){
        squalls[sq_index].address[i] = address[i];
    }
    squalls[sq_index].rssi = rssi; 
  }
}

int main () {
    char adv_buf[10]; 
    rf233_init(0xab, 0xbc, 0xcd);

    ble_advs_allow(adv_buf, sizeof(adv_buf));
    ble_advs_subscribe(adv_callback, adv_buf);
    ble_advs_start();

    while(1) {
      delay_ms(10000);
      send_to_receiver();
    }
}

