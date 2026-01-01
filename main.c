#include "raylib.h"
#include "rlgl.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define grid_size 171

int pixel_size = 6;
int framerate = 60;
int reset = 10;
int screenHeight, screenWidth;
int time = 0;
int mid;
Color col = {252, 243, 117, 255};

struct grid {
  bool st;
  Color col;

} grid[grid_size][grid_size];

int mouseX, mouseY, x, y;
bool alt, alt_ms, alt_auto, alt_drain;
uint32_t rng_state = 0xdeadbeef;
void drawGrid();
void drawUpdate();
void incLogic();
void init();
void resetBot();
bool coinFlip();
void cellOp(int i, int j);
void activateDrain();

int main() {

  // grid[50][50] = true;
  screenHeight = (grid_size)*pixel_size;
  screenWidth = (grid_size - 1) * pixel_size;

  SetTargetFPS(framerate);
  InitWindow(screenWidth, screenHeight, "SAMD");
  init();
  alt = true;
  alt_auto = true;
  alt_drain = false;
  while (!WindowShouldClose()) {

    BeginDrawing();
    ClearBackground(BLACK);
    // grid[30][50] = true;

    if (IsKeyPressed(KEY_A)) {
      alt_auto = !alt_auto;
      printf("Alt-auto Toggled");
    }

    if (alt_auto) {
      if (time < 400)
        col = (Color){158, 152, 77, 255};
      else if (time < 800)
        col = (Color){228, 219, 104, 255};
      else if (time < 1200)
        col = (Color){255, 250, 182, 255};
    }

    if (IsKeyPressed(KEY_D)) {
      alt_drain = !alt_drain;
    }

    if (alt_drain) {
      activateDrain();
    }

    mouseX = GetMouseX();
    mouseY = GetMouseY();

    x = (int)((float)mouseX / (float)pixel_size);
    y = (int)((float)mouseY / (float)pixel_size);

    if (IsKeyPressed(KEY_C)) {
      if (alt) {
        col = (Color){158, 152, 77, 255};
        alt = !alt;
      } else {
        col = (Color){228, 219, 104, 255};
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

      if (time == 1200) {
        time = 0;
      }
      time++;
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
  if (grid[i][j].st == true && i < grid_size - 1 && j >= 0 &&
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

void activateDrain() {

  mid = (int)(grid_size / 2);
  grid[grid_size - 1][mid].st = false;
  grid[grid_size - 1][mid + 2].st = false;
  grid[grid_size - 1][mid - 2].st = false;
  grid[grid_size - 1][mid + 4].st = false;
  grid[grid_size - 1][mid - 4].st = false;
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
