#include "raylib.h"
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

void renderVram(const uint8_t* videoRam) {
    for (int x = 0; x < SPACE_INVADERS_SCREEN_WIDTH; x++) {
        for (int y = 0; y < SPACE_INVADERS_SCREEN_HEIGHT; y++) {
            int byteIdx = (x * 32) + ((255 - y) / 8);
            int bitIdx = (255 - y) % 8;
           
            uint8_t byte = videoRam[byteIdx];
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

int main(int argc, char* argv[])
{
    auto rom = loadROM("/home/everton/Development/cpp_projects/space-invaders/data/invaders.rom");
    SpaceInvadersMachine machine = SpaceInvadersMachine(rom.data());

    InitWindow(SPACE_INVADERS_SCREEN_WIDTH * 2, SPACE_INVADERS_SCREEN_HEIGHT * 2, "Space Invaders Emulator");
    SetTargetFPS(60);

    initScreen();

    while (!WindowShouldClose())
    {
        updateInput(machine.getControllerInput());
        machine.tick();
        renderVram(machine.getVideoRam());

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextureEx(arcadeScreenTexture, (Vector2) {0, 0}, 0, 2, WHITE);

        EndDrawing();
    }

    UnloadTexture(arcadeScreenTexture);

    CloseWindow();

    return 0;
}