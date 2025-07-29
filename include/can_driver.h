//
// Created by toxo on 07/07/25.
//

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <pico/stdlib.h>

uint8_t can_init(void);
void can_send(void);
void can_receive(void);

#endif //CAN_DRIVER_H
