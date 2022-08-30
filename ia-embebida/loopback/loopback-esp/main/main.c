#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "driver/uart.h"

#define MESSAGE_MAX_LEN 10

void app_main(void)
{
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  ESP_ERROR_CHECK(uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
  esp_vfs_dev_uart_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
  esp_vfs_dev_uart_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);

  char message[MESSAGE_MAX_LEN];
  int messageLen = 0;
  uint8_t data;
  int rxBytes = 0;

  while (1)
  {
    rxBytes = uart_read_bytes(CONFIG_ESP_CONSOLE_UART_NUM, &data, 1, 20);
    if (rxBytes > 0)
    {
      char chr = (char)data;
      message[messageLen] = chr;
      if (chr == '\n')
      {
        uart_write_bytes(CONFIG_ESP_CONSOLE_UART_NUM, message, strlen(message));
        for (int i = 0; i < MESSAGE_MAX_LEN; i++)
          message[i] = '\0';
        messageLen = 0;
      }
      else
      {
        messageLen++;
      }
    }
  }
}