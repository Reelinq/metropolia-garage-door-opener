cmake_minimum_required(VERSION 3.13)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(blink C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
	main.cpp
	devices/Stepper.cpp
	devices/Stepper.h
	devices/RotaryEncoder.cpp
	devices/RotaryEncoder.h
	devices/LimitSwitch.cpp
	devices/LimitSwitch.h
	devices/led.cpp
	devices/led.h
	logic/Door.cpp
	logic/Door.h
	logic/Calibration.cpp
	logic/Calibration.h
	logic/Controller.cpp
	logic/Controller.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
	devices logic
)

target_link_libraries(${PROJECT_NAME}
	pico_stdlib pico_util hardware_gpio
)

pico_add_extra_outputs(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
