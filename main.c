#include "raylib.h"
#include "rlgl.h"
#include <stdbool.h>
#include <stdint.h>
// #include <stdlib.h>
//  #include <stdlib.h>

// #include <stdio.h>

#define grid_size 170

int pixel_size = 6;
int framerate = 60;
int reset = 10;
int screenHeight, screenWidth;
int time = 0;
Color col = LIGHTGRAY;
// bool grid[grid_size][grid_size] = {0};
//
struct grid {
  bool st;
  Color col;

} grid[grid_size][grid_size];

int mouseX, mouseY, x, y;
bool alt, alt_ms;
uint32_t rng_state = 0xdeadbeef;
void drawGrid();
void drawUpdate();
void incLogic();
void init();
void resetBot();
bool coinFlip();
void cellOp(int i, int j);

int main() {

  // grid[50][50] = true;
  screenHeight = grid_size * pixel_size;
  screenWidth = grid_size * pixel_size;

  SetTargetFPS(framerate);
  InitWindow(screenWidth, screenHeight, "SAMD");
  init();
  alt = true;
  while (!WindowShouldClose()) {

    BeginDrawing();

    ClearBackground(BLACK);
    // grid[30][50] = true;

    // if (time < 400)
    //   col = PINK;
    // else if (time < 800)
    //   col = YELLOW;
    // else if (time < 1200)
    //   col = VIOLET;
    //

    mouseX = GetMouseX();
    mouseY = GetMouseY();

    x = (int)((float)mouseX / (float)pixel_size);
    y = (int)((float)mouseY / (float)pixel_size);

    if (IsKeyPressed(KEY_C)) {
      if (alt) {
        col = GRAY;
        alt = !alt;
      } else {
        col = LIGHTGRAY;
        alt = !alt;
      }
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      alt_ms = !alt_ms;
    }

    if (grid[y][x].st == false && alt_ms) {
      grid[y][x].st = true;
      grid[y][x + 2].st = true;
      grid[y][x - 2].st = true;
      grid[y][x + 4].st = true;
      grid[y][x - 4].st = true;

      grid[y][x].col = col;
      grid[y][x + 2].col = col;
      grid[y][x - 2].col = col;
      grid[y][x + 4].col = col;
      grid[y][x - 4].col = col;

      // grid[y - 2][x].st = true;
      // grid[y - 2][x + 1].st = true;
      // grid[y - 2][x - 1].st = true;
      // grid[y - 2][x - 3].st = true;
      // grid[y - 2][x + 3].st = true;
    } else if (grid[y][x].st == true && alt_ms) {
      grid[y][x].st = false;
      grid[y][x + 2].st = false;
      grid[y][x - 2].st = false;
      grid[y][x + 4].st = false;
      grid[y][x - 4].st = false;
    }

    // drawGrid();
    if (IsKeyPressed(KEY_R))
      resetBot();

    drawUpdate();
    incLogic();

    if (time == 1200) {
      time = 0;
    }
    time++;
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
    }
  }
}

void incLogic() {
  for (int i = grid_size - 2; i >= 0; i--) {
    bool leftToRight = (i % 2 == 0);
    if (leftToRight) {
      for (int j = 0; j < grid_size; j++) {
        cellOp(i, j);
      }
    } else {
      for (int j = grid_size - 1; j >= 0; j--) {
        cellOp(i, j);
      }
    }
  }
}

void cellOp(int i, int j) {
  if (grid[i][j].st == true && i < grid_size - 1 && j > 0 &&
      j < grid_size - 1) {

    if (grid[i + 1][j].st == false) {

      grid[i][j].st = false;
      grid[i + 1][j].st = true;
      grid[i + 1][j].col = grid[i][j].col;

    } else if (grid[i + 1][j - 1].st == false && grid[i + 1][j].st == true &&
               coinFlip()) {
      grid[i][j].st = false;
      grid[i + 1][j - 1].st = true;
      grid[i + 1][j - 1].col = grid[i][j].col;
    } else if (grid[i + 1][j + 1].st == false && grid[i + 1][j].st == true &&
               coinFlip()) {

      grid[i][j].st = false;
      grid[i + 1][j + 1].st = true;
      grid[i + 1][j + 1].col = grid[i][j].col;
    }

    // else if (grid[i + 1][j].st == true && grid[i][j - 1].st == true &&
    //          grid[i + 1][j - 1].st == true &&
    //          grid[i + 1][j + 1].st == false) {
    //   grid[i][j].st = false;
    //   grid[i + 1][j + 1].st = true;
    // }
    //
    // else if (grid[i + 1][j].st == true && grid[i][j + 1].st == true &&
    //          grid[i + 1][j + 1].st == true &&
    //          grid[i + 1][j - 1].st == false) {
    //   grid[i][j].st = false;
    //   grid[i + 1][j - 1].st = true;
    // }
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
                      grid[i][j].col);
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

bool coinFlip() {
  rng_state ^= rng_state << 13;
  rng_state ^= rng_state >> 17;
  rng_state ^= rng_state << 5;
  return rng_state & 1;
}
