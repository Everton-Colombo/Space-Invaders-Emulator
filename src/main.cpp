#include "raylib.h"
#include "si_controller_input.hpp"
#include "space_invaders_machine.hpp"
#include "utils.hpp"

Color pixelBuffer[SPACE_INVADERS_SCREEN_WIDTH * SPACE_INVADERS_SCREEN_HEIGHT];
Texture2D arcadeScreenTexture;

void updateInput(SpaceInvadersControllerInput& controllerInput) {
    controllerInput.setInputState(COIN, IsKeyDown(KEY_C));
    controllerInput.setInputState(P1_START, IsKeyDown(KEY_ENTER));
    controllerInput.setInputState(P1_SHOOT, IsKeyDown(KEY_W));
    controllerInput.setInputState(P1_LEFT, IsKeyDown(KEY_A));
    controllerInput.setInputState(P1_RIGHT, IsKeyDown(KEY_D));
    controllerInput.setInputState(P2_START, IsKeyDown(KEY_SPACE));
    controllerInput.setInputState(P2_SHOOT, IsKeyDown(KEY_UP));
    controllerInput.setInputState(P2_LEFT, IsKeyDown(KEY_LEFT));
    controllerInput.setInputState(P2_RIGHT, IsKeyDown(KEY_RIGHT));
}

void initScreen() {
    Image img = GenImageColor(SPACE_INVADERS_SCREEN_WIDTH, SPACE_INVADERS_SCREEN_HEIGHT, BLACK);
    arcadeScreenTexture = LoadTextureFromImage(img);
    UnloadImage(img);
}

class SpaceInvadersDisplayRaylib : public ISpaceInvadersDisplay {
public:
    void renderVram(const uint8_t* vram) override {
        for (int x = 0; x < SPACE_INVADERS_SCREEN_WIDTH; x++) {
            for (int y = 0; y < SPACE_INVADERS_SCREEN_HEIGHT; y++) {
                int byteIdx = (x * 32) + ((255 - y) / 8);
                int bitIdx = (255 - y) % 8;
            
                uint8_t byte = vram[byteIdx];
                bool isPixelOn = (byte >> bitIdx) & 1;
                int bufferIdx = (y * SPACE_INVADERS_SCREEN_WIDTH) + x;

                if (isPixelOn) {
                    pixelBuffer[bufferIdx] = WHITE;

                    if (y >= 32 && y <= 63)
                        pixelBuffer[bufferIdx] = RED;
                    else if (y >= 184 && (y <= 239 || (x >= 25 && x <= 135)))
                        pixelBuffer[bufferIdx] = GREEN;
                } else {
                    pixelBuffer[bufferIdx] = BLACK;
                }
            }
        }

        UpdateTexture(arcadeScreenTexture, pixelBuffer);
    }
};

class SpaceInvadersAudioPlayerRaylib : public ISpaceInvadersAudioPlayer {
private:
    Music ufo;
    Sound shot, playerDie, invaderDie, fleetMov1, fleetMov2, fleetMov3, fleetMov4, ufoHit;

    void startUfo() override { PlayMusicStream(ufo); }
    void stopUfo() override { StopMusicStream(ufo); }
    void playShot() override { PlaySound(shot); }
    void playPlayerDie() override { PlaySound(playerDie); }
    void playInvaderDie() override { PlaySound(invaderDie); }
    void playFleetMov1() override { PlaySound(fleetMov1); }
    void playFleetMov2() override { PlaySound(fleetMov2); }
    void playFleetMov3() override { PlaySound(fleetMov3); }
    void playFleetMov4() override { PlaySound(fleetMov4); }
    void playUfoHit() override { PlaySound(ufoHit); }

public:
    SpaceInvadersAudioPlayerRaylib(
        Music ufo, Sound shot, Sound playerDie, Sound invaderDie,
        Sound fleetMov1, Sound fleetMov2, Sound fleetMov3, Sound fleetMov4, Sound ufoHit
    ) : ufo(ufo), shot(shot), playerDie(playerDie), invaderDie(invaderDie),
        fleetMov1(fleetMov1), fleetMov2(fleetMov2), fleetMov3(fleetMov3), fleetMov4(fleetMov4), ufoHit(ufoHit) {
            ufo.looping = true;
            SetMusicVolume(ufo, 0.5);
        }
    
    ~SpaceInvadersAudioPlayerRaylib() {
        UnloadMusicStream(ufo);
        UnloadSound(shot);
        UnloadSound(playerDie);
        UnloadSound(invaderDie);
        UnloadSound(fleetMov1);
        UnloadSound(fleetMov2);
        UnloadSound(fleetMov3);
        UnloadSound(fleetMov4);
        UnloadSound(ufoHit);
    }

    void update() { UpdateMusicStream(ufo); }
};

int main(int argc, char* argv[])
{
    InitWindow(SPACE_INVADERS_SCREEN_WIDTH * 2, SPACE_INVADERS_SCREEN_HEIGHT * 2, "Space Invaders Emulator");
    InitAudioDevice();
    SetTargetFPS(60);

    auto rom = loadROM("/home/everton/Development/cpp_projects/space-invaders/data/invaders.rom");
    auto display = SpaceInvadersDisplayRaylib();
    auto audioPlayer = SpaceInvadersAudioPlayerRaylib(
        LoadMusicStream("data/sounds/0ext.wav"),
        LoadSound("data/sounds/1.wav"),
        LoadSound("data/sounds/2.wav"),
        LoadSound("data/sounds/3.wav"),
        LoadSound("data/sounds/4.wav"),
        LoadSound("data/sounds/5.wav"),
        LoadSound("data/sounds/6.wav"),
        LoadSound("data/sounds/7.wav"),
        LoadSound("data/sounds/8.wav")
    );
    SpaceInvadersMachine machine = SpaceInvadersMachine(rom.data(), &display, &audioPlayer);

    initScreen();

    while (!WindowShouldClose())
    {
        updateInput(machine.getControllerInput());
        machine.tick();
        audioPlayer.update();

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextureEx(arcadeScreenTexture, (Vector2) {0, 0}, 0, 2, WHITE);

        EndDrawing();
    }

    UnloadTexture(arcadeScreenTexture);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}