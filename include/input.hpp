#pragma once

#include <array>
#include <cstdint>
#include <optional>

using namespace std;

class Input {
    private:
        array<bool, 16> keys{};
        optional<uint8_t> pendingEvent;

        bool isValidKey(uint8_t key) const;

    public:

        void updateKeyState(const uint8_t key, bool state);
        bool getKeyState(uint8_t key) const;
        optional<uint8_t> consumePendingEvent();
};