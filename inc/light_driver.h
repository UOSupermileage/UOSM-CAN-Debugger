//
// Created by toxo on 2026-03-14.
//

#ifndef UOSM_CAN_DEBUGGER_LIGHT_DRIVER_H
#define UOSM_CAN_DEBUGGER_LIGHT_DRIVER_H

#include "ApplicationTypes.h"
#define BLINK_DELAY 600 //Will blink a bit less than once every half second.

void lightInit();

// TODO: Create getters and setters to access data
//Getters
flag_status_t getLeftTurnStatus();
flag_status_t getRightTurnStatus();
flag_status_t getHazardsStatus();
flag_status_t getHeadlightsStatus();
flag_status_t getLowBeamsStatus();
flag_status_t getBrakeLightsStatus();

//Setters
void setLightsStatus(uint32_t lights);
void setBrakesStatus(flag_status_t);

//Functions for front lights
void setLeftTurn(flag_status_t enabled);
void setRightTurn(flag_status_t enabled);
void setHazards(flag_status_t enabled);
void setHeadlights(flag_status_t enabled);
void RunningLightsEnabled(flag_status_t enabled);

//Functions for brake lights
void setRunningLights();
void setBrakeLights();

#endif //UOSM_CAN_DEBUGGER_LIGHT_DRIVER_H