#include <stdio.h>
#include <pico/stdio.h>
#include <hardware/gpio.h>
#include <tusb.h>
#include "network.h"
#include "light_driver.h"

#define FRONT_LIGHT

int main() {

    stdio_init_all();

    // Turn on onboard LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, 0);

    // MCP251FXD requires 3ms to warm up
    sleep_ms(100);

    printf("Initializing CAN\n");

    while (!CAN_Init()) {
        sleep_ms(10);
    }

    printf("Listening for CAN\n");

    flag_status_t blink;
    uint32_t blink_delay = BLINK_DELAY; //Blink interval in milliseconds
    // current time
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    // previous time
    uint32_t previous_time = 0;
    lightInit();
    uint32_t counter = 0;
    flag_status_t print = Set;
    while (true) {
        CAN_Receive();

        current_time = to_ms_since_boot(get_absolute_time());

        // if (tud_cdc_available()) {
        //     char buf[64];
        //     int count = tud_cdc_read(buf, sizeof(buf));
        //     buf[count] = '\0';
        //
        //     char send[100];
        //     sprintf(send, "Received: %s\n", buf);
        //     tud_cdc_write_str(send);
        //     tud_cdc_write_flush();
        //     CAN_Send();
        // }

        // Print Light Status
        if (print) {
            if (counter % 100 == 0) { // Print every 100 iterations to avoid spamming
                printf("=== LIGHT STATUS ===\n");
                printf(" • Hazards: %s\n", getHazardsStatus() == Set ? "ON" : "OFF");
                printf(" • Left Turn Signal: %s\n", getLeftTurnStatus() == Set ? "ON" : "OFF");
                printf(" • Right Turn Signal: %s\n", getRightTurnStatus() == Set ? "ON" : "OFF");
                printf(" • Headlights: %s\n", getHeadlightsStatus() == Set ? "ON" : "OFF");
                printf(" • Low Beams: %s\n", getLowBeamsStatus() == Set ? "ON" : "OFF");
                counter = 0;
            }
        }
        // Code for front lights
        if (getHazardsStatus() == Set) {
            setHazards(blink == Set);
        } else {
            setLeftTurn(getLeftTurnStatus() == Set && blink == Set);
            setRightTurn(getRightTurnStatus() == Set && blink == Set);
        }

//        //Proper get and set logic
        setHeadlights(getHeadlightsStatus() == Set);
#ifdef FRONT_LIGHT
        setLowBeams(getLowBeamsStatus() == Set);
#endif
        RunningLightsEnabled(Clear);

        //Update blink flag
        if (current_time - previous_time >= blink_delay) {
            previous_time = current_time;
            if (blink == Set) {
                blink = Clear;
            } else {
                blink = Set;
            }
        }

        sleep_ms(5);
        counter++;
    }
}
