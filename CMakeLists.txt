cmake_minimum_required(VERSION 3.27)

set(ENV{PICO_SDK_PATH} ${CMAKE_SOURCE_DIR}/pico-sdk)
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

set(NAME UOSM_CAN_Debugger)

project(${NAME} C CXX ASM)

# Must initialize Pico SDK before creating executable
pico_sdk_init()

set(CMAKE_C_STANDARD 23)


file(GLOB SOURCES src/*.c UOSM-Core/*.*)

add_executable(${NAME} ${SOURCES})
target_include_directories(${NAME} PUBLIC inc UOSM-Core/Inc UOSM-Core/Drivers UOSM-Core/Modules UOSM-Core/Utils)

target_link_libraries(${NAME}
        pico_stdlib
        hardware_spi
        hardware_gpio
)

pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 1)

target_compile_definitions(${NAME} PRIVATE
        PICO_DEFAULT_UART_RX_PIN=1
        PICO_DEFAULT_UART_TX_PIN=0
)

#add_custom_command(TARGET ${NAME} POST_BUILD
#        COMMAND ../pico_install.sh ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.uf2
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        COMMENT "Flashing Pico after successful build..."
#)