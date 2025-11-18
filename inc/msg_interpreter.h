//
// Created by crdup on 2025-11-01.
//

#ifndef UOSM_CAN_DEBUGGER_MSG_INTERPRETER_H
#define UOSM_CAN_DEBUGGER_MSG_INTERPRETER_H

#include <stdint.h>
#include <stdbool.h>

//main function when data is received
void MsgInterpreter_ProcessMessage(uint16_t messageID, const uint8_t *data);

#endif //UOSM_CAN_DEBUGGER_MSG_INTERPRETER_H