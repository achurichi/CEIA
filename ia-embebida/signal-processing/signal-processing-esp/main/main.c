#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "driver/uart.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"

#define WINDOW_SIZE 20

static intr_handle_t handle_console;

uint64_t sum;
uint16_t elementCount;
double mean;
uint32_t circularBuffer[WINDOW_SIZE];
uint32_t *circularBufferAccessor = circularBuffer;

void appendToBuffer(uint32_t value)
{
  *circularBufferAccessor = value;
  circularBufferAccessor++;
  if (circularBufferAccessor >= circularBuffer + WINDOW_SIZE)
    circularBufferAccessor = circularBuffer;
}

double simpleMovingAverage(int value)
{
  sum -= *circularBufferAccessor;
  sum += value;
  appendToBuffer(value);
  if (elementCount < WINDOW_SIZE)
    ++elementCount;
  return (double)sum / elementCount;
}

static void IRAM_ATTR
uart_intr_handle(void *arg)
{
  uint8_t len = UART0.status.rxfifo_cnt;
  uint8_t data[4];
  if (len >= 4)
  {
    for (int i = 0; i < 4; i++)
    {
      data[i] = UART0.fifo.rw_byte;
    }
    uint32_t sample = ((uint32_t)data[3] << 24) |
                      ((uint32_t)data[2] << 16) |
                      ((uint32_t)data[1] << 8) |
                      data[0];

    uint32_t result = (uint32_t)simpleMovingAverage(sample);

    uart_write_bytes(UART_NUM_0, &result, sizeof(uint32_t));
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