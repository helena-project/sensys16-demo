#include <string.h>
#include <timer.h>

#include "rf233-const.h"
#include "rf233-config.h"
#include "rf233-arch.h"
#include "trx_access.h"
#include "rf233.h"

typedef struct {
  int8_t rssi;
  int imix;
} squall;

squall squalls[5];

int callback(void*, int, uint8_t); 

void print_squalls() {
  char buf[6];
  for (int i = 0; i < 5; i++) {
    buf[i] = squalls[i].imix;
    if (buf[i] == 0) {
      buf[i] = 0xff;
    }
  }
  buf[5] = 0;
}

int main() { 
  rf233_init(0xab, 0xbc, 0xcd);
  rf233_rx_data(callback);
}

int callback(void* buffer, int len, uint8_t src) {
  int8_t* rssis = (uint8_t*) buffer; 
  for (int i = 0; i < 5; i ++) {
    if (squalls[i].rssi > rssis[i]) {
      squalls[i].rssi = rssis[i];
      squalls[i].imix = src;
    }
  }
  print_squalls();
  return 0; 
}
