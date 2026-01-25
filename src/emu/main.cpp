#include "raylib.h"
#include "space_invaders.hpp"
#include "utils.hpp"

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

int main(int argc, char* argv[])
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Space Invaders Emulator");
    SetTargetFPS(60);

    Image arcadeScreen = GenImageColor(256, 224, BLACK);
    // ImageDrawPixel(&image, x, y, WHITE);
    Texture2D arcadeScreenTexture = LoadTextureFromImage(arcadeScreen);

    SpaceInvadersMachine machine = SpaceInvadersMachine(loadROM(argv[1]).data());

    while (!WindowShouldClose())
    {
        updateInput(machine.getControllerInput());

        UnloadTexture(arcadeScreenTexture);
        arcadeScreenTexture = LoadTextureFromImage(arcadeScreen);

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextureEx(arcadeScreenTexture, (Vector2) {0, 0}, 0, 2, WHITE);

        EndDrawing();
    }

    UnloadImage(arcadeScreen);

    CloseWindow();

    return 0;
}