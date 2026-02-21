//
// Created by Jeremy Cote on 2025-04-22.
//

#ifndef UOSM_CAN_DEBUGGER_NETWORK_H
#define UOSM_CAN_DEBUGGER_NETWORK_H
#include "ApplicationTypes.h"

#include <stdbool.h>

bool CAN_Init();

void CAN_Receive();
void CAN_Send();
void CAN_Send_Throttle(uint16_t*);
void Flush();
#endif //UOSM_CAN_DEBUGGER_NETWORK_H
