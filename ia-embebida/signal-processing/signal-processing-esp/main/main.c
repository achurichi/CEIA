#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "driver/uart.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#include "../../cfunctions.c"

#define WINDOW_SIZE 4

double x_coef[] = {0.00031579, 0.00094736, 0.00094736, 0.00031579};

double y_coef[] = {1., -2.71506105, 2.46928626, -0.75169892};

// double x_coef[] = {
//     6.85369982e-09,
//     4.79758987e-08,
//     1.43927696e-07,
//     2.39879494e-07,
//     2.39879494e-07,
//     1.43927696e-07,
//     4.79758987e-08,
//     6.85369982e-09,
// };

// double y_coef[] = {
//     1.,
//     -6.35957907,
//     17.36022673,
//     -26.36667776,
//     24.06161832,
//     -13.19284752,
//     4.02392261,
//     -0.52666242,
// };

static intr_handle_t handle_console;

uint32_t in[WINDOW_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};
uint32_t out[WINDOW_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0};

uint32_t filterSample(uint32_t sample)
{
  for (int i = WINDOW_SIZE - 1; i >= 1; i--) // Se rotan las entradas
    in[i] = in[i - 1];
  in[0] = sample; // La nueva lectura se almacena en la posición 0

  double output = (double)in[0] * x_coef[0];

  for (int i = 1; i < WINDOW_SIZE; i++) // Se calcula la salida
    output += (double)in[i] * x_coef[i] - (double)out[i - 1] * y_coef[i - 1];
  for (int i = WINDOW_SIZE - 1; i >= 1; i--) // Se rotan las salidas
    out[i] = out[i - 1];

  out[0] = (uint32_t)(output > 0 ? output : 0);

  return out[0];
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

    uint32_t result = filterSample(sample);

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