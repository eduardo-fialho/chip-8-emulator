#include "./include/sound.hpp"

#include <stdexcept>

void Sound::setFrequency(uint16_t frequency) {
    
    if(frequency == 0) throw invalid_argument("Frequency cannot be zero");
    if(frequency > 20000) throw invalid_argument("Frequency too high");
    this->frequency = frequency;
}
        
uint16_t Sound::getFrequency() const {
    return frequency;
}
        
void Sound::setActive(bool state) {
    active = state;
}
        
bool Sound::isActive() const {
    return active;
}