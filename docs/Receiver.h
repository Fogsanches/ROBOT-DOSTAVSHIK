#pragma once
#include "ACmdReceiver.h"
#include <iostream>

class FooCmdReceiver : public ACmdReceiver {
public:
    std::string receive() override {
        std::cout << "Введите команду (forward/left/right/stop + время в мс): ";
        std::string command;
        std::getline(std::cin, command);
        return command;
    }
};
