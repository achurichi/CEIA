#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "driver/uart.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"

static intr_handle_t handle_console;

// Decission Tree
uint8_t predict(uint8_t *data)
{
  uint8_t zeroCrossing = data[0];
  uint8_t maximum = data[1];
  uint8_t simetry = data[2];

  if (maximum <= 49)
  {
    if (maximum <= 39)
    {
      return 1;
    }
    else
    {
      return simetry <= 95 ? 0 : 1;
    }
  }
  else
  {
    if (simetry <= 126)
    {
      return zeroCrossing <= 4 ? 0 : 2;
    }
    else
    {
      return simetry <= 213 ? 2 : 0;
    }
  }
}

static void IRAM_ATTR
uart_intr_handle(void *arg)
{
  uint8_t len = UART0.status.rxfifo_cnt;
  uint8_t data[3];
  if (len >= 3)
  {
    for (int i = 0; i < 3; i++)
    {
      data[i] = UART0.fifo.rw_byte;
    }

    uint8_t result = predict(data);

    uart_write_bytes(UART_NUM_0, &result, sizeof(uint8_t));
  }
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