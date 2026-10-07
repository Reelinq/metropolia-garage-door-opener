cmake_minimum_required(VERSION 3.13)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(blink C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

file(GLOB MQTT_SOURCES "paho.mqtt.embedded-c/MQTTPacket/src/*.c")

add_executable(${PROJECT_NAME}
	header.h
	main.cpp
	devices/Stepper.cpp
	devices/Stepper.h
	devices/RotaryEncoder.cpp
	devices/RotaryEncoder.h
	devices/LimitSwitch.cpp
	devices/LimitSwitch.h
	devices/Led.cpp
	devices/Led.h
	devices/Button.cpp
	devices/Button.h
	devices/Eeprom.cpp
	devices/Eeprom.h
	logic/Door.cpp
	logic/Door.h
	logic/Calibration.cpp
	logic/Calibration.h
	logic/Controller.cpp
	logic/Controller.h
	logic/DoorStorage.cpp
	logic/DoorStorage.h
	network/Mqtt.cpp
	network/Mqtt.h
    network/IPStack.cpp
    network/IpStack.h
    network/lwipopts.h
	network/RemoteControl.cpp
	network/RemoteControl.h
    ${MQTT_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    devices logic ${CMAKE_CURRENT_LIST_DIR}
    paho.mqtt.embedded-c/MQTTPacket/src
    paho.mqtt.embedded-c/MQTTClient/src
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	WIFI_SSID="${WIFI_SSID}"
	WIFI_PASSWORD="${WIFI_PASSWORD}"
	BROKER_IP="${BROKER_IP}"
	BROKER_PORT=${BROKER_PORT}
)

target_link_libraries(${PROJECT_NAME}
	pico_stdlib pico_util hardware_gpio hardware_i2c pico_cyw43_arch_lwip_poll
)

pico_add_extra_outputs(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
