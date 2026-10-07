cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(blink C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

file(GLOB MQTT_SOURCES "paho.mqtt.embedded-c/MQTTPacket/src/*.c")

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME}
	main.cpp
	${SOURCES}
	${MQTT_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
	src src/hardware src/door src/remote src/net
	paho.mqtt.embedded-c/MQTTPacket/src
	paho.mqtt.embedded-c/MQTTClient/src
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
	WIFI_SSID=\"$ENV{WIFI_SSID}\"
	WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
)

target_link_libraries(${PROJECT_NAME}
	pico_stdlib pico_util hardware_gpio hardware_i2c hardware_watchdog
	pico_cyw43_arch_lwip_poll
)

pico_add_extra_outputs(${PROJECT_NAME})

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)
