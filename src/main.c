/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <stdio.h>

#include "can_driver.h"
#include <tusb.h>

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 500
#endif

// Perform initialisation
int pico_led_init(void) {
  // A device like Pico that uses a GPIO for the LED will define
  // PICO_DEFAULT_LED_PIN so we can use normal GPIO functionality to turn the
  // led on and off
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  return PICO_OK;
}

// Turn the led on or off
void pico_set_led(bool led_on) {
  gpio_put(PICO_DEFAULT_LED_PIN, led_on);
}

void can_fc() {
    can_receive();

    can_send();

    if (tud_cdc_available()) {
      char buf[64];
      int count = tud_cdc_read(buf, sizeof(buf));
      buf[count] = '\0';

      char send[100];
      sprintf(send, "Received: %s\n", buf);
      tud_cdc_write_str(send);
      tud_cdc_write_flush();
    }

    // sleep_ms(5);
}

int main() {
  stdio_init_all();
  int rc = pico_led_init();
  hard_assert(rc == PICO_OK);

  pico_set_led(true);

  printf("Pico LED init done\n");

  // MCP251FXD requires 3ms to warm up
  sleep_ms(100);

  printf("Initializing CAN\n");

  while (!can_init()) {
    printf("Initializing CAN Fail\n");
    sleep_ms(10);
  }
  printf("CAN initialized\n");

  pico_set_led(false);

  while (true) {
    pico_set_led(true);
    // printf("LED ON\n");
    can_fc();
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    // printf("LED OFF\n");
    can_fc();
    sleep_ms(LED_DELAY_MS);
  }
}
