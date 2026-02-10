#pragma once
#include <stdint.h>

class ISpaceInvadersAudioPlayer {
private:
    uint8_t prevOutPort3 = 0, prevOutPort5 = 0;

    virtual void startUfo() = 0;
    virtual void stopUfo() = 0;
    virtual void playShot() = 0;
    virtual void playPlayerDie() = 0;
    virtual void playInvaderDie() = 0;
    virtual void playExtraLife() = 0;
    virtual void playFleetMov1() = 0;
    virtual void playFleetMov2() = 0;
    virtual void playFleetMov3() = 0;
    virtual void playFleetMov4() = 0;
    virtual void playUfoHit() = 0;

public:
    void interpretOutPort3(uint8_t outPort3) {
        if (outPort3 != prevOutPort3) {
            if ((outPort3 & 0b00000001) && !(prevOutPort3 & 0b00000001)) { startUfo(); }
            else if (!(outPort3 & 0b00000001) && (prevOutPort3 & 0b00000001)) { stopUfo(); }
            if ((outPort3 & 0b00000010) && !(prevOutPort3 & 0b00000010)) { playShot(); }
            if ((outPort3 & 0b00000100) && !(prevOutPort3 & 0b00000100)) { playPlayerDie(); }
            if ((outPort3 & 0b00001000) && !(prevOutPort3 & 0b00001000)) { playInvaderDie(); }
            if ((outPort3 & 0b00010000) && !(prevOutPort3 & 0b00010000)) { playExtraLife(); }
         
            prevOutPort3 = outPort3;
        }
    }

    void interpretOutPort5(uint8_t outPort5) {
        if (outPort5 != prevOutPort5) {
            if ((outPort5 & 0b00000001) && !(prevOutPort5 & 0b00000001)) { playFleetMov1(); }
            if ((outPort5 & 0b00000010) && !(prevOutPort5 & 0b00000010)) { playFleetMov2(); }
            if ((outPort5 & 0b00000100) && !(prevOutPort5 & 0b00000100)) { playFleetMov3(); }
            if ((outPort5 & 0b00001000) && !(prevOutPort5 & 0b00001000)) { playFleetMov4(); }
            if ((outPort5 & 0b00010000) && !(prevOutPort5 & 0b00010000)) { playUfoHit(); }            
         
            prevOutPort5 = outPort5;
        }
    }
};