#include <stdlib.h>
#include <stdio.h>

#include "rf233-const.h"
#include "rf233-config.h"
#include "rf233-arch.h"
#include "trx_access.h"
#include "rf233.h"

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

int callback(void* buffer, int buffer_len) {

  if (buffer_len < 3){
    return 1;
  }

  uint8_t* bytes = (uint8_t*) buffer;
  char squall_id = bytes[0];
  int rsi = (int)bytes[1] | (int)bytes[2];

  //TODO: do something with the id and rsi of the closest squall

  return 0;
}

int main(void) {

  rf233_init(0xab, 0xbc, 0xcd);
  rf233_rx_data(callback);

}

