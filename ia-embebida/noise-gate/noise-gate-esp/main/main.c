#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "driver/uart.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#include "../../cfunctions.c"

#define OPEN_LIMIT 3500
#define CLOSE_LIMIT 300
#define HOLD 0.3
#define SAMPLERATE 22050

static intr_handle_t handle_console;

Config config;

static void IRAM_ATTR
uart_intr_handle(void *arg)
{
  uint8_t len = UART0.status.rxfifo_cnt;
  uint8_t data[2];
  if (len >= 2)
  {
    for (int i = 0; i < 2; i++)
    {
      data[i] = UART0.fifo.rw_byte;
    }
    uint16_t sample = ((uint16_t)data[1] << 8) | data[0];

    bool isOpen = noiseGate(sample, (Config *)arg);
    uint16_t result = isOpen ? sample : toUint16(0);

    uart_write_bytes(UART_NUM_0, &result, sizeof(uint16_t));
  }
  uart_flush(UART_NUM_0);
  uart_clear_intr_status(UART_NUM_0, UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);
}

void app_main(void)
{
  config.open = toUint16(OPEN_LIMIT);
  config.close = toUint16(CLOSE_LIMIT);
  config.samplesHold = HOLD * SAMPLERATE;
  config.openCounter = 0;
  config.isOpen = false;

  ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0));
  uart_isr_free(UART_NUM_0);
  uart_isr_register(UART_NUM_0, uart_intr_handle, &config, ESP_INTR_FLAG_IRAM, &handle_console);
  uart_enable_rx_intr(UART_NUM_0);

  while (1)
  {
    vTaskDelay(1000);
  }
}