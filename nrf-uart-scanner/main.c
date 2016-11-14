#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "ble.h"
#include "ble_advdata.h"
#include "ble_stack_handler_types.h"
#include "app_error.h"
#include "softdevice_handler.h"
#include "nordic_common.h"
#include "nrf_delay.h"
#include "nrf_drv_uart.h"
#include "nrf_gpio.h"
#include "app_uart.h"

#include "ble_ser.h"

#define LED 13
#define INT 2

/*
 * The SPI tx buffers are structured in a queue implemented as a a ring-array.
 * When `txq_head == txq_tail`, there is nothing to send. Otherwise,
 * `txq_head` has the next command to send, and txq_head is where to
 * enqueue the next command.
 */

#define QUEUE_PKT_LEN 64
#define TX_QUEUE_SIZE 10

uint8_t rx_buf[QUEUE_PKT_LEN];

int txq_head = 0;
int txq_tail = 0;
uint8_t tx_queue[TX_QUEUE_SIZE][QUEUE_PKT_LEN];
uint8_t *tx_cur;

uint8_t* dequeue_cmd() {
  uint8_t* result;
  if (txq_tail == txq_head) {
    result = NULL;
  } else {
    result = tx_queue[txq_head];
    txq_head = (txq_head + 1) % TX_QUEUE_SIZE;
  }
  return result;
}

/*
 * enqueue_cmd
 *
 * Enqueue a command for transmission by setting it in the tail of the queue
 * and incrementing the tail. If the tail wraps around and reaches the head,
 * return an error.
 */
int enqueue_cmd(uint8_t* cmd, size_t len) {
  if ((txq_tail + 1) % TX_QUEUE_SIZE == txq_head) {
    return -1;
  }
  memcpy(tx_queue[txq_tail], cmd, len);
  txq_tail = (txq_tail + 1) % TX_QUEUE_SIZE;
  return 0;
}

ble_gap_scan_params_t scan_params = {
  .active = 0,
  .selective = 0,
  .p_whitelist = NULL,
  .interval = 0x0640,
  .window = 0x0320,
  .timeout = 0
};

void app_error_handler(uint32_t error_code,
                       uint32_t line_num, const uint8_t *file) {
  nrf_gpio_pin_set(LED);
  char buf[256];
  int len = snprintf(buf, 256, "0x%x: %d@%s\n", error_code, line_num, file);
  nrf_drv_uart_tx(buf, len);
}

void assert_nrf_callback(uint16_t line_num, const uint8_t *file_name) {
  app_error_handler(0xff, line_num, file_name);
}

void notify_advertisement(ble_gap_evt_adv_report_t adv_report) {
  if (adv_report.peer_addr.addr[5] == 0xC0 &&
      adv_report.peer_addr.addr[4] == 0x98 &&
      adv_report.peer_addr.addr[3] == 0xE5) {
    uint8_t cmd[QUEUE_PKT_LEN];
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = BLE_ADVERTISE;
    memcpy(cmd + 1, adv_report.peer_addr.addr,
        sizeof(adv_report.peer_addr.addr));
    cmd[7] = adv_report.rssi;
    cmd[8] = adv_report.dlen;
    memcpy(cmd + 9, adv_report.data, adv_report.dlen);
    enqueue_cmd(cmd, sizeof(cmd));
  }
}

void notify_debug(int evt_id) {
  uint8_t cmd[QUEUE_PKT_LEN];
  cmd[0] = BLE_DEBUG;
  int len = sprintf(cmd + 2, "BLE EVT: %d\n", evt_id);
  cmd[1] = (uint8_t)len;
  enqueue_cmd(cmd, sizeof(cmd));
}

void ble_handler(ble_evt_t *evt) {
  switch(evt->header.evt_id) {
    case BLE_GAP_EVT_ADV_REPORT:
      nrf_gpio_pin_toggle(LED);
      notify_advertisement(evt->evt.gap_evt.params.adv_report);
      break;
    default:
      notify_debug(evt->header.evt_id);
      break;
  }
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t err_code;

    // Initialize the SoftDevice handler module.
    SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_RC_250_PPM_8000MS_CALIBRATION, false);

    // Enable BLE stack.
    ble_enable_params_t ble_enable_params;
    memset(&ble_enable_params, 0, sizeof(ble_enable_params));
#if (defined(S130) || defined(s132))
    ble_enable_params.gatts_enable_params.attr_tab_size   = BLE_GATTS_ATTR_TAB_SIZE_DEFAULT;
#endif
    ble_enable_params.gatts_enable_params.service_changed = false;
#ifdef S120
    ble_enable_params.gap_enable_params.role              = BLE_GAP_ROLE_CENTRAL;
#endif

    err_code = sd_ble_enable(&ble_enable_params);
    APP_ERROR_CHECK(err_code);

    // Register with the SoftDevice handler module for BLE events.
    APP_ERROR_CHECK(softdevice_ble_evt_handler_set(&ble_handler));

}

static void uart_handler(nrf_drv_uart_event_t *p_event, void *p_context) {
  switch(p_event->type) {
    case NRF_DRV_UART_EVT_TX_DONE:
      break;
    default:
      break;
  }
}

static void uart_init() {
  uint32_t err_code;

  nrf_drv_uart_config_t config = NRF_DRV_UART_DEFAULT_CONFIG;

  err_code = nrf_drv_uart_init(&config, uart_handler);
  APP_ERROR_CHECK(err_code);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
  nrf_gpio_cfg_output(LED);
  nrf_gpio_cfg_output(INT);


  uart_init();
  ble_stack_init();

  APP_ERROR_CHECK(sd_ble_gap_scan_start(&scan_params));
  while (1) {
    tx_cur = dequeue_cmd();
    if (tx_cur != NULL) {
      APP_ERROR_CHECK(nrf_drv_uart_tx(tx_cur, QUEUE_PKT_LEN));
    }
    APP_ERROR_CHECK(sd_app_evt_wait());
  }
}

