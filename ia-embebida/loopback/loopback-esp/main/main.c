#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"

static intr_handle_t handle_console;

static void IRAM_ATTR uart_intr_handle(void *arg)
{
  uint8_t len = UART0.status.rxfifo_cnt;
  uint8_t i = 0;
  uint8_t *data = (uint8_t *)malloc(len + 1);
  data[len] = '\0';
  while (len)
  {
    data[i++] = UART0.fifo.rw_byte;
    len--;
  }
  uart_write_bytes(UART_NUM_0, (char *)data, strlen((char *)data));
  uart_flush(UART_NUM_0);
  uart_clear_intr_status(UART_NUM_0, UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);
}

void app_main(void)
{
  ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0));
  uart_isr_free(UART_NUM_0);
  uart_isr_register(UART_NUM_0, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console);
  uart_enable_rx_intr(UART_NUM_0);

  while (1)
  {
    vTaskDelay(1000);
  }
}