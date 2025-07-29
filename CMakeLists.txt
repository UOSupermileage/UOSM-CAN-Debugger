cmake_minimum_required(VERSION 3.12)

# Import Pico SDK
include(${CMAKE_SOURCE_DIR}/pico-sdk/external/pico_sdk_import.cmake)
set(CMAKE_C_FLAGS_DEBUG "-g -O0")

project(pico-can-fd VERSION 1.0.0 LANGUAGES C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Must initialize Pico SDK before adding targets
pico_sdk_init()

file(GLOB SOURCES
        src/*.c
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(${PROJECT_NAME} 
  pico_stdlib
  hardware_spi
  hardware_gpio
)

pico_enable_stdio_usb(${PROJECT_NAME} 1) # Enable USB serial output for printf
pico_enable_stdio_uart(${PROJECT_NAME} 1) # Enable UART serial output for printf
pico_add_extra_outputs(${PROJECT_NAME}) # Create .uf2 file
