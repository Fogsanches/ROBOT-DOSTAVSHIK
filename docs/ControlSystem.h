#pragma once
#include "ACmdReceiver.h"
#include "AEngine.h"
#include <iostream>
#include <string>
#include <sstream>

class ControlSystem {
public:
    ControlSystem(AEngine& engine, ACmdReceiver& receiver)
        : engine(engine), receiver(receiver) {}
    
    void run() {
        while (true) {
            std::string msg = receiver.receive();
            if (msg == "exit") break;
            
            parseAndExecute(msg);
        }
    }

private:
    AEngine& engine;
    ACmdReceiver& receiver;
    
    void parseAndExecute(const std::string& msg) {
        std::istringstream iss(msg);
        std::string cmd;
        int time_ms = 0;
        
        iss >> cmd;
        if (cmd != "stop") iss >> time_ms;
        
        if (cmd == "forward") engine.forward(time_ms);
        else if (cmd == "left") engine.left(time_ms);
        else if (cmd == "right") engine.right(time_ms);
        else if (cmd == "stop") engine.stop();
        else std::cerr << "Unknown command: " << cmd << "\n";
    }
};
