#include <stdio.h>
#include "esp_sleep.h"
#include "esp_wifi.h"

void app_main(void)
{
  // Modem sleep
  esp_wifi_set_ps(WIFI_PS_MIN_MODEM);

  // Light sleep
  // esp_light_sleep_start();

  // Deep sleep
  // esp_deep_sleep_start();

  // Hibernation
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF); // Disable RTC IO, sensors and ULP co-processor
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF); // Disable RTC slow memory.
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);   // Disable RTC fast memory
  // esp_deep_sleep_start();
}