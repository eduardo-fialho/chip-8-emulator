#include "./include/renderer.hpp"

#include <stdexcept>

void Renderer::clear() {
    for(array<bool, 64> &a : framebuffer) a.fill(false);
}

const array<array<bool, 64>, 32>& Renderer::getFramebuffer() const {
    return framebuffer;
}

bool Renderer::draw(uint8_t x, uint8_t y, const vector<uint8_t> &sprite) {

    unsigned int height = sprite.size();
    bool collision = false;
    unsigned int s = 0;
    uint8_t mask = 0b10000000;

    for(unsigned int i = y; i < y + height; i++, s++) { 
        for(unsigned int j = x; j < x + 8; j++, mask /= 2) {
            bool state = framebuffer[i % 32][j % 64]; 
            if(static_cast<bool>(mask & sprite[s])) 
                framebuffer[i % 32][j % 64] = !framebuffer[i % 32][j % 64];
            if(state && !framebuffer[i % 32][j % 64]) 
                collision = true;
        }
        mask = 0b10000000;
    }

    return collision;
}