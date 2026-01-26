#include "raylib.h"
#include "space_invaders.hpp"
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

            pixelBuffer[bufferIdx] = isPixelOn ? WHITE : BLACK;
        }
    }

    UpdateTexture(arcadeScreenTexture, pixelBuffer);
}

int main(int argc, char* argv[])
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Space Invaders Emulator");
    SetTargetFPS(60);

    SpaceInvadersMachine machine = SpaceInvadersMachine(loadROM(argv[1]).data());
    initScreen();

    while (!WindowShouldClose())
    {
        updateInput(machine.getControllerInput());
        renderVram(machine.getVideoRam());
        machine.tick();

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextureEx(arcadeScreenTexture, (Vector2) {0, 0}, 0, 2, WHITE);

        EndDrawing();
    }

    UnloadTexture(arcadeScreenTexture);

    CloseWindow();

    return 0;
}