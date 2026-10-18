#pragma once

#include <array>
#include <cstdint>
#include <vector>

using namespace std;

class Renderer {
    private:
       array<array<bool, 64>, 32> framebuffer{{}}; // [y][x]

    public:

        void clear();
        bool draw(uint8_t x, uint8_t y, const vector<uint8_t> &sprite); // Renderizar do outro lado caso ultrapasse a borda e retorna verdadeiro em caso de colisão
        const array<array<bool, 64>, 32>& getFramebuffer() const;
};