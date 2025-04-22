#include <stdio.h>
#include <pico/stdio.h>
#include <hardware/gpio.h>
#include <tusb.h>

int main() {

    stdio_init_all();

    // Turn on onboard LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // MCP251FXD requires 3ms to warm up
    sleep_ms(3);



    while (true) {
        if (tud_cdc_available()) {
            char buf[64];
            int count = tud_cdc_read(buf, sizeof(buf));
            buf[count] = '\0';

            char send[100];
            sprintf(send, "Received: %s\n", buf);
            tud_cdc_write_str(send);
            tud_cdc_write_flush();
        }

        sleep_ms(5);
    }
}
