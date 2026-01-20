#include "raylib.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Space Invaders Emulator");
    SetTargetFPS(60);

    Image arcadeScreen = GenImageColor(256, 224, BLACK);
    // ImageDrawPixel(&image, x, y, WHITE);
    Texture2D arcadeScreenTexture = LoadTextureFromImage(arcadeScreen);

    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

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