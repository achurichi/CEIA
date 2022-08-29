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
  char chr;

  while (1)
  {
    for (int i = 0; i < MESSAGE_MAX_LEN; i++)
    {
      scanf("%c", &chr);
      if (chr == '\n')
        break;
      message[i] = chr;
    }

    printf("\nReceived: %s\n", message);

    for (int i = 0; i < MESSAGE_MAX_LEN; i++)
      message[i] = '\0';
  }
}