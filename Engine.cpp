#include "FooEngine.h"
#include <unistd.h>

#define MQTT_HOST "localhost"
#define MQTT_PORT 1883
#define MQTT_TOPIC "/motor/command"

FooEngine::FooEngine() {
    mosquitto_lib_init();
    char clientid[24];
    snprintf(clientid, 23, "engine_%d", getpid());
    mosq = mosquitto_new(clientid, true, this);

    if (mosq) {
        mosquitto_message_callback_set(mosq, FooEngine::message_callback);
        if (mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 60) != MOSQ_ERR_SUCCESS) {
            std::cerr << "Ошибка подключения" << std::endl;
        }

        mosquitto_subscribe(mosq, nullptr, MQTT_TOPIC, 0);

        mqtt_thread = std::thread([this]() {
            while (running) {
                int rc = mosquitto_loop(mosq, -1, 1);
                if (running && rc) {
                    std::cerr << "Ошибка соединения, переподключение..." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                    mosquitto_reconnect(mosq);
                }
            }
        });
    }
}

FooEngine::~FooEngine() {
    running = false;
    if (mqtt_thread.joinable())
        mqtt_thread.join();
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

void FooEngine::message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    FooEngine* self = static_cast<FooEngine*>(obj);
    std::string payload(static_cast<char*>(message->payload), message->payloadlen);
    self->handleCommand(payload);
}

void FooEngine::handleCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string cmd;
    int time = 0;

    iss >> cmd;
    if (cmd != "stop") iss >> time;

    if (cmd == "forward") forward(time);
    else if (cmd == "left") left(time);
    else if (cmd == "right") right(time);
    else if (cmd == "stop") stop();
    else std::cout << "Неизвестная команда: " << cmd << std::endl;
}

void FooEngine::simulateDelay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void FooEngine::forward(int time_ms) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Forward " << time_ms << " ms" << std::endl;
    simulateDelay(time_ms);
}

void FooEngine::left(int time_ms) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Left " << time_ms << " ms" << std::endl;
    simulateDelay(time_ms);
}

void FooEngine::right(int time_ms) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Right " << time_ms << " ms" << std::endl;
    simulateDelay(time_ms);
}

void FooEngine::stop() {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Stop" << std::endl;
}
