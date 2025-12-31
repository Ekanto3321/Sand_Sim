#include "const.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdbool.h>
#include <stdint.h>
// #include <stdlib.h>

// #include <stdio.h>

#define grid_size 150

uint32_t rng_state = 0xdeadbeef; // for fast rng

int pixel_size = 6;
int framerate = 60;
int reset = 10;
int screenHeight, screenWidth;
// bool grid[grid_size][grid_size] = {0};
//
struct grid {
  bool st;
  Color col;

} grid[grid_size][grid_size];

int mouseX, mouseY, x, y;
bool alt, alt_ms;

void drawGrid();
void drawUpdate();
void incLogic();
void init();
void resetBot();
bool coinFlip();

uint32_t xorshift32();

int main() {

  // grid[50][50] = true;
  screenHeight = grid_size * pixel_size;
  screenWidth = grid_size * pixel_size;

  SetTargetFPS(framerate);
  InitWindow(screenWidth, screenHeight, "SAMD");
  init();
  while (!WindowShouldClose()) {

    alt = true;
    BeginDrawing();

    ClearBackground(DARKGRAY);
    // grid[30][50] = true;

    mouseX = GetMouseX();
    mouseY = GetMouseY();

    x = (int)((float)mouseX / (float)pixel_size);
    y = (int)((float)mouseY / (float)pixel_size);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      alt_ms = !alt_ms;
    }

    if (grid[y][x].st == false && alt_ms) {
      grid[y][x].st = true;
      grid[y][x + 2].st = true;
      grid[y][x - 2].st = true;
      grid[y][x + 4].st = true;
      grid[y][x - 4].st = true;

    } else {
      grid[y][x].st = false;
      grid[y][x + 2].st = false;
      grid[y][x - 2].st = false;
    }
    // printf("%d %d \n", x, y);

    // drawGrid();
    if (IsKeyPressed(KEY_R))
      resetBot();

    drawUpdate();
    incLogic();

    // drawUpdate();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

void init() {

  for (int i = 0; i < grid_size; i++) {
    for (int j = 0; j < grid_size; j++) {

      grid[i][j].st = false;
      grid[i][j].col = LIGHTGRAY;
    }
  }
}

void incLogic() {

  for (int i = grid_size; i > 0; i--) {
    for (int j = 0; j < grid_size; j++) {

      if (grid[i][j].st == true && i < grid_size - 1) {

        if (grid[i + 1][j].st == false) {

          grid[i][j].st = false;
          grid[i + 1][j].st = true;
        } else if (grid[i + 1][j + 1].st == false &&
                   grid[i + 1][j].st == true && alt) {

          grid[i][j].st = false;
          grid[i + 1][j + 1].st = true;
          alt = !alt;
        } else if (grid[i + 1][j - 1].st == false &&
                   grid[i + 1][j].st == true && !alt) {
          grid[i][j].st = false;
          grid[i + 1][j - 1].st = true;
          alt = !alt;
        }
      }
    }
  }
}

void resetBot() {
  for (int i = grid_size - reset; i < grid_size; i++) {
    for (int j = 0; j < grid_size; j++) {

      grid[i][j].st = false;
    }
  }
}
void drawUpdate() {

  for (int i = 0; i < grid_size; i++) {
    for (int j = 0; j < grid_size; j++) {

      if (grid[i][j].st == true) {
        DrawRectangle(j * pixel_size, i * pixel_size, pixel_size, pixel_size,
                      LIGHTGRAY);
      }
    }
  }
}

void drawGrid() {
  for (int i = 0; i < screenWidth; i += pixel_size)
    DrawLine(0, i, screenWidth, i, DARKGRAY);
  for (int i = 0; i < screenHeight; i += pixel_size)
    DrawLine(i, 0, i, screenHeight, DARKGRAY);
}

// rng stuff from main

uint32_t xorshift32() {
  rng_state ^= rng_state << 13;
  rng_state ^= rng_state >> 17;
  rng_state ^= rng_state << 5;

  // Tempering - unbiased uniform 32-bit
  uint32_t t = rng_state;
  t ^= t >> 16;
  t *= 0x85ebca6bU;
  t ^= t >> 13;
  t *= 0xc2b2ae35U;
  t ^= t >> 16;

  return t;
}

bool coinFlip() {
  return xorshift32() & 1; // Now unbiased
}
