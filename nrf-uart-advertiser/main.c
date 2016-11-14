/*
 * Send an advertisement periodically
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_gpio.h"
#include "ble_advdata.h"
#include "nordic_common.h"
#include "softdevice_handler.h"
#include "ble_debug_assert_handler.h"
#include "led.h"

#include "app_util_platform.h"
#include "nrf_uart.h"
#include "nrf_drv_common.h"

#include "simple_ble.h"
#include "eddystone.h"

// Name is maximum length that fits in an advertisement
char name[20] = "imix";

#define IMIX_EDDYSTONE_URL "j2x.us/imix"

#define APP_COMPANY_IDENTIFIER 0x02E0
#define IMIX_SERVICE_ID 0x1F
#define IMIX_VERSION_NUM 1

//#define UART_RX_PIN 14
#define UART_RX_PIN 3
#define LED 13

#define IMIX_UART_PACKET_LEN 5
#define IMIX_ADV_MAX_LEN 20
static uint8_t imix_adv_data[IMIX_ADV_MAX_LEN] = {0};

// Intervals for advertising and connections
static simple_ble_config_t ble_config = {
    .platform_id       = 0x00,              // used as 4th octect in device BLE address
    .device_id         = DEVICE_ID_DEFAULT,
    .adv_name          = name,
    .adv_interval      = MSEC_TO_UNITS(500, UNIT_0_625_MS),
    .min_conn_interval = MSEC_TO_UNITS(500, UNIT_1_25_MS),
    .max_conn_interval = MSEC_TO_UNITS(1000, UNIT_1_25_MS)
};

void ble_error(uint32_t error_code) {
    led_init(13);
    led_on(13);
}

// prototypes
void update_manufdata_adv(void);
void start_manufdata_adv(void);

// *** UART STUFF ***

static void uart_enable(void) {
    // enable module, clear events, start receiving
    nrf_uart_enable(NRF_UART0);
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_ERROR);
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);
    nrf_uart_task_trigger(NRF_UART0, NRF_UART_TASK_STARTRX);

    // enable interrupts on UART RX
    nrf_uart_int_enable(NRF_UART0, NRF_UART_INT_MASK_RXDRDY);
}

void uart_init (void) {
    // apply config
    nrf_gpio_cfg_input(UART_RX_PIN, NRF_GPIO_PIN_NOPULL);
    nrf_uart_txrx_pins_set(NRF_UART0, 0, UART_RX_PIN);
    nrf_uart_baudrate_set(NRF_UART0, UART_BAUDRATE_BAUDRATE_Baud115200);
    nrf_uart_configure(NRF_UART0, NRF_UART_PARITY_EXCLUDED, NRF_UART_HWFC_DISABLED);

    // interrupts enable
    nrf_drv_common_irq_enable(UART0_IRQn, APP_IRQ_PRIORITY_LOW);
}

void UART0_IRQHandler (void) {
    static uint16_t uart_index = 0;

    // data is available
    nrf_uart_event_clear(NRF_UART0, NRF_UART_EVENT_RXDRDY);
    uint8_t uart_data = nrf_uart_rxd_get(NRF_UART0);

    // copy data into buffer
    // NOTE: offset of 2 to go after the service ID
    //  and version number
    imix_adv_data[uart_index+2] = uart_data;
    uart_index++;

    // toggle LED!
    led_toggle(LED);

    if (uart_index >= IMIX_UART_PACKET_LEN) {
        // update advertisement
        update_manufdata_adv();

        // reset uart packet data
        uart_index = 0;
    }
}

// *** END OF UART STUFF ***


// *** START OF ADV STUFF ***

void init_adv_data (void) {
    memset(imix_adv_data, 0x00, IMIX_ADV_MAX_LEN);

    // set service ID
    imix_adv_data[0] = IMIX_SERVICE_ID;

    // set version
    imix_adv_data[1] = IMIX_VERSION_NUM;

    // dummy data
    imix_adv_data[2] = 11;
    imix_adv_data[3] = 22;
    imix_adv_data[4] = 33;
    imix_adv_data[5] = 44;
    imix_adv_data[6] = 77;
}

void update_manufdata_adv (void) {
    // double check that these are still set
    imix_adv_data[0] = IMIX_SERVICE_ID;
    imix_adv_data[1] = IMIX_VERSION_NUM;

    // just call start again
    start_manufdata_adv();
}

void start_manufdata_adv (void) {
    // advertise manufacturer specific data payload
    ble_advdata_manuf_data_t manuf_specific_data;
    manuf_specific_data.company_identifier = APP_COMPANY_IDENTIFIER;
    manuf_specific_data.data.p_data = imix_adv_data;
    manuf_specific_data.data.size   = 2 + IMIX_UART_PACKET_LEN;

    eddystone_with_manuf_adv(IMIX_EDDYSTONE_URL, &manuf_specific_data);
}

// *** END OF ADV STUFF


int main(void) {

    // Setup BLE
    simple_ble_init(&ble_config);

    // Initialize UART data LED
    led_init(LED);

    // start advertising with default data
    init_adv_data();
    start_manufdata_adv();

    // set up the UART
    uart_init();
    uart_enable();

    while (1) {
        power_manage();
    }
}

