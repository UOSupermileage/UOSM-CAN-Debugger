//
// Created by toxo on 2026-03-14.
//

#include "light_driver.h"

#include <hardware/gpio.h>

#define LOW_BEAM_PIN 2
#define HIGH_BEAM_PIN 7
#define LEFT_TURN_PIN 5
#define RIGHT_TURN_PIN 6
#define RUNNING_LIGHTS_R_PIN 0
#define RUNNING_LIGHTS_G_PIN 1
#define RUNNING_LIGHTS_B_PIN 3

static volatile brightness_t left_turn_lights;
static volatile brightness_t right_turn_lights;
static volatile brightness_t high_beam_lights;
static volatile brightness_t low_beam_lights;

static volatile ms_t turning_lights_frequency;

//States
static lights_status_t lights_status;
static flag_status_t brakes_status;

void lightInit() {
    // Low beam pin gp2
    gpio_init(LOW_BEAM_PIN);
    gpio_set_dir(LOW_BEAM_PIN, GPIO_OUT);
    gpio_put(LOW_BEAM_PIN, 0);
    // High beam pin gp7
    gpio_init(HIGH_BEAM_PIN);
    gpio_set_dir(HIGH_BEAM_PIN, GPIO_OUT);
    gpio_put(HIGH_BEAM_PIN, 0);
    // Left turn signal pin gp5
    gpio_init(LEFT_TURN_PIN);
    gpio_set_dir(LEFT_TURN_PIN, GPIO_OUT);
    gpio_put(LEFT_TURN_PIN, 0);
    // Right turn signal pin gp6
    gpio_init(RIGHT_TURN_PIN);
    gpio_set_dir(RIGHT_TURN_PIN, GPIO_OUT);
    gpio_put(RIGHT_TURN_PIN, 0);
    // Running lights RGB pins gp0, gp1, gp3
    gpio_init(RUNNING_LIGHTS_R_PIN);
    gpio_set_dir(RUNNING_LIGHTS_R_PIN, GPIO_OUT);
    gpio_put(RUNNING_LIGHTS_R_PIN, 0);
    gpio_init(RUNNING_LIGHTS_G_PIN);
    gpio_set_dir(RUNNING_LIGHTS_G_PIN, GPIO_OUT);
    gpio_put(RUNNING_LIGHTS_G_PIN, 0);
    gpio_init(RUNNING_LIGHTS_B_PIN);
    gpio_set_dir(RUNNING_LIGHTS_B_PIN, GPIO_OUT);
    gpio_put(RUNNING_LIGHTS_B_PIN, 0);

}

//Global setter
void setLightsStatus(uint32_t lights){
    lights_status.all = lights;
}
void setBrakesStatus(flag_status_t status){
    brakes_status = status;
}

//Getters
flag_status_t getLeftTurnStatus(){
    return lights_status.left_turn_enabled;
}
flag_status_t getRightTurnStatus(){
    return lights_status.right_turn_enabled;
}
flag_status_t getHeadlightsStatus(){
    return lights_status.headlights_enabled;
}
flag_status_t getLowBeamsStatus() {
    return lights_status.low_beams_enabled;
}
flag_status_t getHazardsStatus(){
    return lights_status.hazards_enabled;
}
flag_status_t getBrakeLightsStatus(){
    // return brakes_status;
    // return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_SET ? Set : Clear;
}

//Abstract functions over HAL
void setLeftTurn(flag_status_t enabled) {
    // HAL_GPIO_WritePin(LeftTurn_port, LeftTurn_pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
    gpio_put(LEFT_TURN_PIN, enabled ? 1 : 0);
}

void setHazards(flag_status_t enabled) {
    // HAL_GPIO_WritePin(LeftTurn_port, LeftTurn_pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // HAL_GPIO_WritePin(RightTurn_port, RightTurn_pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
    gpio_put(LEFT_TURN_PIN, enabled ? 1 : 0);
    gpio_put(RIGHT_TURN_PIN, enabled ? 1 : 0);
}

void setRightTurn(flag_status_t enabled) {
    // HAL_GPIO_WritePin(RightTurn_port, RightTurn_pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
    gpio_put(RIGHT_TURN_PIN, enabled ? 1 : 0);
}

void setHeadlights(flag_status_t enabled) {
// #ifdef BRUCE_FRONT_LIGHTS
//     HAL_GPIO_WritePin(Headlights1_port, Headlights1_pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(Headlights2_port, Headlights2_pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
// #endif
    gpio_put(HIGH_BEAM_PIN, enabled ? 1 : 0);
}
void setLowBeams(flag_status_t enabled) {
    gpio_put(LOW_BEAM_PIN, enabled ? 1 : 0);
}

void RunningLightsEnabled(flag_status_t enabled){
// #ifdef BRUCE_REAR_LIGHTS
//     HAL_GPIO_WritePin(RunningLights_port, RunningLights_pin, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
// #endif
    //  gpio_put(LOW_BEAM_PIN, enabled ? 1 : 0);
    gpio_put(RUNNING_LIGHTS_R_PIN, enabled ? 1 : 0);
    gpio_put(RUNNING_LIGHTS_G_PIN, enabled ? 1 : 0);
    gpio_put(RUNNING_LIGHTS_B_PIN, enabled ? 1 : 0);
}
void BrakeLightsEnabled(flag_status_t enabled){
// #ifdef BRUCE_REAR_LIGHTS
//     HAL_GPIO_WritePin(BrakeLights_port, BrakeLights_pin, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
// #endif
}

void setRunningLights() {
    //RunningLightsEnabled(lights_status.low_beams_enabled);
}

void setBrakeLights() {
    BrakeLightsEnabled(brakes_status);
}