#include <stdlib.h>
#include "ble.h"
#include "simple_ble.h"

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

void ble_evt_adv_report(ble_evt_t* ble_evt) {
  ble_gap_evt_t *gap_evt = &ble_evt->evt.gap_evt;
  ble_gap_evt_adv_report_t *adv_report = &gap_evt->params.adv_report;

  int8_t rssi = adv_report->rssi;
  printf("RSSI: %d\n", rssi);
  print_hex(adv_report->peer_addr.addr, 6);
  print_hex(adv_report->data, 31);
}

int main(void) {
    simple_ble_scan_start();
}

