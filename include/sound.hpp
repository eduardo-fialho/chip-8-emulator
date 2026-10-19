#pragma once

#include <cstdint>

using namespace std;

class Sound {
    private:
        
        uint16_t frequency = 400; // 400Hz (Default)
        bool active = false;

    public:

        void setFrequency(uint16_t frequency);
        uint16_t getFrequency() const;
        void setActive(bool state);
        bool isActive() const;

};