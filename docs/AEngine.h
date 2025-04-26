#pragma once

class AEngine {
public:
    virtual ~AEngine() = default;
    virtual void forward(int time_ms) = 0;
    virtual void left(int time_ms) = 0;
    virtual void right(int time_ms) = 0;
    virtual void stop() = 0;
};
