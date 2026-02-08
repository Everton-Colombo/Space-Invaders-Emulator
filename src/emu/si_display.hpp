#pragma once
#include <stdint.h>

class ISpaceInvadersDisplay {
public:
    virtual void renderVram(const uint8_t* vram) = 0;
};