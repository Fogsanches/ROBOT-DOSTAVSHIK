#pragma once
#include "AEngine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <mosquitto.h>
#include <string>
#include <sstream>

class FooEngine : public AEngine {
public:
    FooEngine();
    ~FooEngine();

    void forward(int time_ms) override;
    void left(int time_ms) override;
    void right(int time_ms) override;
    void stop() override;

    static void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

private:
    void handleCommand(const std::string& input);
    void simulateDelay(int ms);

    std::mutex mtx;
    struct mosquitto* mosq = nullptr;
    std::thread mqtt_thread;
    bool running = true;
};
