#pragma once

#define MQTT_TOPIC_COMMAND "garage/door/command"
#define MQTT_TOPIC_STATUS "garage/door/status"
#define MQTT_TOPIC_RESPONSE "garage/door/response"

#define BROKER_IP "10.161.4.63"
#define BROKER_PORT 1883

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/timer.h"
#include "pico/cyw43_arch.h"

#include "IPStack.h"
#include "Countdown.h"
#include "MQTTClient.h"

class Mqtt {
	private:
		static void messageArrived(MQTT::MessageData& md);
		const char* ssid;
		const char* pw;
		const char* ip;
		int port;
		IPStack ipstack;
		MQTT::Client<IPStack, Countdown> client;
		MQTTPacket_connectData data;
		int rc;
	public:
		Mqtt(const char* ssid, const char * pw, const char * ip, int port);
		bool init();
		bool subscribe(const char *topic);
		bool publish(const char *topic, const char *message);
		const char* checkMsg();
};
