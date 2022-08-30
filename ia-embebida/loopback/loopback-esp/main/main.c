#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "driver/uart.h"

#define MESSAGE_MAX_LEN 10

static QueueHandle_t uart_queue;

static void uart_task(void *arg)
{
  uint8_t *data = (uint8_t *)malloc(MESSAGE_MAX_LEN);
  uart_event_t event;

  for (;;)
  {
    if (xQueueReceive(uart_queue, &event, portMAX_DELAY))
    {
      switch (event.type)
      {
      case UART_DATA:
      {
        int len = uart_read_bytes(CONFIG_ESP_CONSOLE_UART_NUM, data, event.size, portMAX_DELAY);
        if (len)
        {
          *(data + event.size) = '\0';
          uart_write_bytes(CONFIG_ESP_CONSOLE_UART_NUM, (char *)data, strlen((char *)data));
        }
      }
      break;
      default:
        break;
      }
    }
  }
}

void app_main(void)
{
  ESP_ERROR_CHECK(uart_driver_install(CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 32, &uart_queue, 0));
  esp_vfs_dev_uart_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);

  xTaskCreate(uart_task, "uart_event_task", 2048, NULL, 12, NULL);

  while (1)
  {
    vTaskDelay(100);
  }
}