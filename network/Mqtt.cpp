#include "mqtt.h"

void Mqtt::messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n", (char *) message.payload);
}

Mqtt::Mqtt(const char * ssid, const char * pw, const char * ip, int port) : 
	ssid(ssid),
	pw(pw),
	ip(ip),
	port(port),
	ipstack(IPStack(ssid, pw)),
	client(MQTT::Client<IPStack, Countdown>(ipstack)),
	data(MQTTPacket_connectData_initializer)
{
	rc = 0;
}

// initalises the program, must be run before any other mqtt method, returns true if works
bool Mqtt::init() {
	rc = ipstack.connect(ip, port);
	if (rc != 1) {
		printf("rc from TCP connect is %d\n", rc);
	}

	printf("MQTT connecting\n");
	data.MQTTVersion = 3;
	data.clientID.cstring = (char *) "PicoW";
	rc = client.connect(data);
	if (rc != 0) {
        	printf("rc from MQTT connect is %d\n", rc);
		printf("Attempt to connect failed");
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
        	printf("Not connected...\n");
        	rc = client.connect(data);
        	if (rc != 0) {
        		printf("rc from MQTT connect is %d\n", rc);
			return false;
	        }
        }
        char buf[100];
        int rc = 0;
        MQTT::Message message;
        message.retained = false;
        message.dup = false;
        message.payload = (void *) buf;
        sprintf(buf, msg);
        printf("%s\n", buf);
        message.qos = MQTT::QOS1;
        message.payloadlen = strlen(buf) + 1;
        rc = client.publish(topic, message);
        printf("Publish rc=%d\n", rc);
	return true;
}

//checks any arriving messages, does not work fully yet
void Mqtt::checkMsg() {
	cyw43_arch_poll();
	client.yield(100);
}
