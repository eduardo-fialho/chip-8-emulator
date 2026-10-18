#include "./include/input.hpp"

#include <stdexcept>

bool Input::isValidKey(uint8_t key) const {
    if(key < 16) return true;
    return false;
}

void Input::updateKeyState(const uint8_t key, bool state) {
    if(!isValidKey(key)) throw out_of_range("Invalid key");
    if(!keys[key] && state) pendingEvent.emplace(key);
    keys[key] = state;
}

bool Input::getKeyState(uint8_t key) const {
    if(!isValidKey(key)) throw out_of_range("Invalid key");
    return keys[key];
}

optional<uint8_t> Input::consumePendingEvent() {
    if(pendingEvent.has_value()) {
        optional<uint8_t> aux;
        aux.emplace(pendingEvent.value());
        pendingEvent.reset();
        return aux;
    } 
    return pendingEvent;
}