#include <mosquitto.h>
#include <iostream>
#include <cstring>

int main() {
    mosquitto_lib_init();
    struct mosquitto* mosq = mosquitto_new("cmd_sender", true, nullptr);
    mosquitto_connect(mosq, "localhost", 1883, 60);
    std::string cmd = "forward 1000";
    mosquitto_publish(mosq, nullptr, "/motor/command", cmd.size(), cmd.c_str(), 0, false);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}
