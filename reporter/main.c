#include <stdlib.h>
#include <stdio.h>

#include "rf233-const.h"
#include "rf233-config.h"
#include "rf233-arch.h"
#include "trx_access.h"
#include "rf233.h"

#define SQUALL_ADDRESS_SIZE 6
#define MAX_COLLECTOR_COUNT 10

void print_hex(const uint8_t *buf, int len) {
  int i;
  char* buf_str = (char*) malloc (2*len + 1);
  char* buf_ptr = buf_str;
  for (i = 0; i < len; i++) {
      buf_ptr += sprintf(buf_ptr, "%02X", buf[i]);
  }
  sprintf(buf_ptr,"\n");
  *(buf_ptr + 1) = '\0';
  printf("%s\n", buf_str);
  free(buf_str);
}

typedef struct Collector {
 uint16_t id;
 char closest_squall[SQUALL_ADDRESS_SIZE];
 char rssi;
} Collector;

static Collector collectors[MAX_COLLECTOR_COUNT];
static int collector_count = 0;

int callback(void* buffer, int buffer_len, uint16_t src, uint16_t dest, uint16_t pan_id) {

  if (buffer_len < 3){
    return 1;
  }

  uint8_t* bytes = (uint8_t*) buffer;

  char squall_address[SQUALL_ADDRESS_SIZE];
  char rssi;

  for (int i = 0; i < SQUALL_ADDRESS_SIZE; i++){
    squall_address[i] = bytes[i];
  }
  rssi = bytes[SQUALL_ADDRESS_SIZE];

  //find the right collector
  int collector_index = -1;
  for (int i = 0; i < MAX_COLLECTOR_COUNT; i++){
    if (collectors[i].id == src){
        collector_index = i;
        break;
    }
  }

  //if it didn't find the right collector, make it create a new one in the next slot
  if (collector_index == -1){
      collector_index = collector_count;
      collector_count ++;
  }

  collectors[collector_index] = { src, address, rssi };

  return 0;
}

int main(void) {

  rf233_init(0xab, 0xbc, 0xcd);
  rf233_rx_data(callback);

}

