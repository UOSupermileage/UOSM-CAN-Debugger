//
// Created by crdup on 2025-11-01.
/*
 * The purpose of these files is to decode the bytes
 * with the information about the different data
 */
//

#include "msg_interpreter.h"
#include <stdio.h>
#include "CANMessageLookUpModule.h"

static void decodeThrottle(const uint8_t *data);
static void decodeSpeed(const uint8_t *data);
static void decodeMotorRPM(const uint8_t *data);
static void decodeEvent(const uint8_t *data);
static void decodeError(const uint8_t *data);
static void decodeVoltage(const uint8_t *data);
static void decodeLights(const uint8_t *data);
static void decodePressure(const uint8_t *data);
static void decodeEfficiency(const uint8_t *data);
static void decodeMotorTemp(const uint8_t *data);


void MsgInterpreter_ProcessMessage(uint16_t messageID, const uint8_t *data)
{
    switch (messageID)
    {
    case THROTTLE_DATA_ID:
        decodeThrottle(data);
        break;

    case SPEED_DATA_ID:
        decodeSpeed(data);
        break;

    case MOTOR_RPM_DATA_ID:
        decodeMotorRPM(data);
        break;

    case EVENT_DATA_ID:
        //fully decode this
        decodeEvent(data);
        break;

    case ERROR_DATA_ID:
        decodeError(data);
        break;

    case CURRENT_VOLTAGE_DATA_ID:
        //fully decode this
        decodeVoltage(data);
        break;

    case LIGHT_DATA_ID:
        decodeLights(data);
        break;

    case PRESSURE_TEMPERATURE_DATA_ID:
        //fully decode this
        decodePressure(data);
        break;

    case EFFICIENCY_DATA_ID:
        decodeEfficiency(data);
        break;

    case MOTOR_TEMPERATURE_DATA_ID:
        //maybe this
        decodeMotorTemp(data);
        break;

    default:
        printf("Unknown Message ID: %d\n", messageID);
        break;
    }
}

static void decodeThrottle(const uint8_t *data)
{
    printf("Throttle\n");
}

static void decodeSpeed(const uint8_t *data)
{
    printf("Speed\n");
}

static void decodeMotorRPM(const uint8_t *data)
{
    printf("MotorRPM\n");
}

static void decodeEvent(const uint8_t *data)
{
    printf("Event\n");
}

static void decodeError(const uint8_t *data)
{
    printf("Error\n");
}

static void decodeVoltage(const uint8_t *data)
{
    printf("Voltage\n");
}

static void decodeLights(const uint8_t *data)
{

    uint32_t raw = (data[0] << 24) | (data[1] << 16) |
                   (data[2] << 8)  | (data[3]);

    lights_status_t status;
    status.all = raw;

    printf("=== LIGHT STATUS ===\n");

    if (status.hazards_enabled)
        printf(" • Hazards: ON\n");

    if (status.left_turn_enabled)
        printf(" • Left Turn Signal: ON\n");

    if (status.right_turn_enabled)
        printf(" • Right Turn Signal: ON\n");

    if (status.headlights_enabled)
        printf(" • Headlights: ON\n");

    if (status.low_beams_enabled)
        printf(" • Low Beams: ON\n");

    if (!status.hazards_enabled && !status.left_turn_enabled &&
        !status.right_turn_enabled && !status.headlights_enabled &&
        !status.low_beams_enabled)
    {
        printf(" • Lights: All OFF\n");
    }

    //printf(" • RGB Value: (%u, %u, %u)\n", status.r, status.g, status.b);

    printf("====================\n");

}

static void decodePressure(const uint8_t *data)
{
    printf("Pressure\n");
}

static void decodeEfficiency(const uint8_t *data)
{
    printf("Efficiency\n");
}

static void decodeMotorTemp(const uint8_t *data)
{
    printf("Motor Temp\n");
}