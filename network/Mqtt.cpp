#include "Mqtt.h"
#include <string.h>

static char received[32];
static bool has_msg = false;

void Mqtt::messageArrived(MQTT::MessageData &md) {
	MQTT::Message &message = md.message;

	size_t n = message.payloadlen < sizeof(received) - 1 ? message.payloadlen : sizeof(received) - 1;
	memcpy(received, message.payload, n);
	received[n] = '\0';
	has_msg = true;

	printf("Received: %s\n", received);
}

Mqtt::Mqtt(const char * ssid, const char * pw, const char * ip, int port) :
	ssid(ssid),
	pw(pw),
	ip(ip),
	port(port),
	ipstack(IPStack(ssid, pw)),
	client(MQTT::Client<IPStack, Countdown>(ipstack, 500)),
	data(MQTTPacket_connectData_initializer)
{
	rc = 0;
}

// initalises the program, must be run before any other mqtt method, returns true if works
bool Mqtt::init() {
	// wait for Wi-Fi + DHCP lease, connecting earlier gives ERR_RTE (-4)
	for (int i = 0; i < 3 && cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP; i++) {
		printf("Wi-Fi not up, retrying\n");
		cyw43_arch_wifi_connect_timeout_ms(ssid, pw, CYW43_AUTH_WPA2_AES_PSK, 10000);
	}

	data.MQTTVersion = 3;
	data.clientID.cstring = (char *) "PicoW";

	for (int attempt = 1; attempt <= 5; attempt++) {
		if (attempt > 1) { ipstack.disconnect(); } // drop the failed socket
		printf("MQTT connecting, attempt %d\n", attempt);
		rc = ipstack.connect(ip, port);
		if (rc == 0) {
			// tcp_connect is asynchronous: let the handshake finish before sending CONNECT
			for (int i = 0; i < 500; i++) {
				cyw43_arch_poll();
				sleep_ms(1);
			}
			rc = client.connect(data);
			if (rc == 0) { break; }
		}
		printf("Attempt %d failed, rc %d\n", attempt, rc);
		sleep_ms(1000);
	}
	if (rc != 0) {
		return false;
	}

	printf("MQTT connected 555\n");
	return true;
}

//subscribes to topic
bool Mqtt::subscribe(const char* topic) {
	rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
	if (rc != 0) {
		printf("rc from MQTT subscribe is %d\n", rc);
	return false;
	}
	printf("MQTT subscribed\n");
	return true;
}

//publishes to topic (you dont need to subscribe to topic to publish in it)
bool Mqtt::publish(const char* topic, const char* msg) {
	if (!client.isConnected()) {
		return false;
	}

	MQTT::Message message;
	message.retained = false;
	message.dup = false;
	message.payload = (void *) msg;
	message.qos = MQTT::QOS1;
	message.payloadlen = strlen(msg);
	rc = client.publish(topic, message);
	printf("Publish rc=%d\n", rc);

	return rc == 0;
}

// services the connection, returns the received message or nullptr
const char* Mqtt::checkMsg() {
	if (!client.isConnected()) { return nullptr; }
	cyw43_arch_poll();

	static uint32_t last_ms = 0;
	uint32_t now = to_ms_since_boot(get_absolute_time());
	if (now - last_ms >= 20) {
		last_ms = now;
		client.yield(1);
	}

	if (!has_msg) { return nullptr; }
	has_msg = false;
	return received;
}
