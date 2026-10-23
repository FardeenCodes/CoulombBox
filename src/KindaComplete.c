/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit
https://creativecommons.org/publicdomain/zero/1.0/

*/
// #include <std>
// //TODO CHECK WALL  COLLIDE WHEN SPAWNING PARTICLE TOO
#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define HEIGHT 900
#define WIDTH 900
#define k 36000
#define RAD 20
typedef struct {
  Vector2 pos, v, acc;
  double charge;
  int mass;
} chargeP;
chargeP *arr = NULL;
int noP = 0;

void drawCharge() {
  for (int i = 0; i < noP; i++) {
    DrawCircleV(arr[i].pos, RAD, arr[i].charge > 0 ? BLUE : RED);
  }
}
int main() {
  // Tell the window to use vsync and work on high DPI displays
  // SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  // Create the window and OpenGL context
  InitWindow(WIDTH, HEIGHT, "Hello Raylib");
  chargeP *tmp = NULL;
  chargeP tmpCharge;
  Vector2 ef = {0, 0}, dir, acc, startPos;
  float r2, u1, u2, U = 0, ke = 0, ovrLap;
  bool invalidP = false;

  SetTargetFPS(60);
  float dt;                    // game loop
  while (!WindowShouldClose()) // run the loop until the user presses ESCAPE
                               // or presses the Close button on the window

  {
    // dt = 1 / 240.0f;
    dt = 1 / 60.0f;
    // dt = GetFrameTime();
    // drawing
    BeginDrawing();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      startPos = GetMousePosition();
    else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
      startPos = GetMousePosition();

    ClearBackground(BLACK);
    // if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      // tmpCharge.pos = GetMousePosition();
      tmpCharge.pos = startPos;
      tmpCharge.charge = +1;
      tmpCharge.mass = +1;
      tmpCharge.acc = (Vector2){0, 0};
      // tmpCharge.v = (Vector2){0, 0};
      tmpCharge.v = Vector2Subtract(GetMousePosition(), startPos);
      for (int i = 0; i < noP; i++) {

        if (CheckCollisionCircles(tmpCharge.pos, RAD, arr[i].pos, RAD)) {
          invalidP = true;
        }
      }
      if (!(invalidP)) {
        noP++;
        chargeP *tmp = (chargeP *)realloc(arr, (noP) * sizeof(chargeP));
        if (tmp == NULL) {
          return 1;
        }
        arr = tmp;
        arr[noP - 1] = tmpCharge;

        // free(tmp);NEVER FREE TMP ITS POINTING TO ARR
      }
    }
    // else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
      // tmpCharge.pos = GetMousePosition();
      tmpCharge.pos = startPos;
      tmpCharge.charge = -1;
      // tmpCharge.charge = 0;
      tmpCharge.mass = +1;
      // tmpCharge.v = (Vector2){0, 0};
      tmpCharge.acc = (Vector2){0, 0};
      tmpCharge.v = Vector2Subtract(GetMousePosition(), startPos);
      if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        tmpCharge.v = Vector2Subtract(GetMousePosition(), startPos);
      }
      for (int i = 0; i < noP; i++) {

        if (CheckCollisionCircles(tmpCharge.pos, RAD + 2, arr[i].pos,
                                  RAD + 2)) {
          invalidP = true;
        }
      }
      if (!(invalidP)) {
        noP++;
        printf("%d", sizeof(chargeP));
        chargeP *tmp = (chargeP *)realloc(arr, (noP) * sizeof(chargeP));
        if (tmp == NULL) {
          return 1;
        }
        arr = tmp;
        arr[noP - 1] = tmpCharge;

        // free(tmp);NEVER FREE TMP ITS POINTING TO ARR
      }

      invalidP = false;
    }
    drawCharge();
    for (int i = 0; i < noP; i++) {
      ef = (Vector2){0, 0};
      for (int j = 0; j < noP; j++) {
        if (i == j) // no E on itself
          continue;
        // if (j == 0) {
        //   dir = Vector2Subtract(arr[i].pos, arr[j].pos);
        //   r2 = Vector2LengthSqr(dir);
        //   dir = Vector2Normalize(dir);
        //   ef = Vector2Scale(dir, k * arr[i].charge * arr[j].charge / r2);
        // } else {
        dir = Vector2Subtract(arr[i].pos, arr[j].pos);
        r2 = Vector2LengthSqr(dir);
        dir = Vector2Normalize(dir);
        ef = Vector2Add(
            ef, Vector2Scale(dir, k * arr[i].charge * arr[j].charge / r2));
        // U += Vector2Length(Vector2Scale(
        // ef, Vector2Length(Vector2Subtract(arr[i].pos, arr[j].pos))));
        // }
        // printf("i=%d  %f\n", i, r2);
      }
      acc = Vector2Scale(ef, 1.0 / arr[i].mass);
      // acc = ef;
      // arr[i].acc = acc;
      arr[i].v = Vector2Add(arr[i].v, Vector2Scale(acc, dt));
      ef = (Vector2){0, 0};
      // if (i == noP - 1)
      // printf("v.x:%f v.y:%f \na.x:%f", arr[i].v.x, arr[i].v.y, acc.x);
    }
    for (int i = 0; i < noP; i++) {
      // printf("v.x=%f  v.y%f\n", arr[i].v.x, arr[i].v.y);
      // arr[i].v = Vector2Add(arr[i].v, Vector2Scale(arr[i].acc, dt));

      // wall collide[CORRECT]
      if (arr[i].pos.x - RAD <= 0)
        // if (arr[i].pos.x + arr[i].v.x - RAD <= 0){
        arr[i].v.x *= -1;
      // arr[i].pos.x = RAD;}
      else if (arr[i].pos.x + RAD >= WIDTH) {
        arr[i].v.x *= -1;
        arr[i].pos.x = WIDTH - RAD;
      }
      if (arr[i].pos.y + RAD >= HEIGHT)
        arr[i].v.y *= -1;
      else if (arr[i].pos.y - RAD <= 0)
        arr[i].v.y *= -1;

      // collision among particles
      for (int j = i + 1; j < noP; j++) {
        // if (CheckCollisionCircles(
        //         Vector2Add(arr[i].pos, Vector2Scale(arr[i].v, dt)), RAD + 2,
        //         Vector2Add(arr[j].pos, Vector2Scale(arr[j].v, dt)), RAD + 2)
        //         &&
        //     i != j) {

        if (CheckCollisionCircles(arr[i].pos, RAD, arr[j].pos, RAD) && i != j) {
          dir = Vector2Normalize(
              Vector2Subtract(arr[i].pos, arr[j].pos)); // pointing toward i
          // SHOULD DIR VECTOR BE WRT PREDICTED POS OR ACTUAL POS
          // dir = Vector2Normalize(Vector2Subtract(
          //     Vector2Add(arr[i].pos, Vector2Scale(arr[i].v, dt)),
          //     Vector2Add(arr[j].pos, Vector2Scale(arr[j].v, dt))));
          u1 = Vector2DotProduct(arr[i].v, dir);

          u2 = Vector2DotProduct(arr[j].v, dir);
          if (Vector2LengthSqr(Vector2Subtract(
                  Vector2Add(arr[i].pos, Vector2Scale(arr[i].v, dt)),
                  Vector2Add(arr[j].pos, Vector2Scale(arr[j].v, dt)))) <
              Vector2LengthSqr(Vector2Subtract(arr[i].pos, arr[j].pos))) {
            // if (u2 - u1 > 0) {
            arr[i].v = Vector2Add(arr[i].v, Vector2Scale(dir, u2 - u1));
            arr[j].v = Vector2Add(arr[j].v, Vector2Scale(dir, u1 - u2));

            // Remove overlap;
            ovrLap = 2 * RAD -
                     (Vector2Length(Vector2Subtract(arr[i].pos, arr[j].pos)));
            arr[i].pos =
                Vector2Add(arr[i].pos, Vector2Scale(dir, ovrLap / 2.0));
            arr[j].pos =
                Vector2Add(arr[j].pos, Vector2Scale(dir, -ovrLap / 2.0));
          }
          // arr[i].pos = Vector2Add(arr[i].pos, Vector2Scale(dir, -1));
          // arr[j].pos = Vector2Add(arr[i].pos, Vector2Scale(dir, 1));

          // arr[i].v = Vector2Scale(arr[i].v, -1);
          // arr[j].v = Vector2Scale(arr[j].v, -1);
        }
      }
      // arr[i].pos = Vector2Add(arr[i].pos, Vector2Scale(arr[i].v, dt));
    }
    for (int i = 0; i < noP; i++) {
      arr[i].pos = Vector2Add(arr[i].pos, Vector2Scale(arr[i].v, dt));
    }
    // // for U calc
    for (int i = 0; i < noP; i++) {
      for (int j = i + 1; j < noP; j++) {
        dir = Vector2Subtract(arr[i].pos, arr[j].pos);
        r2 = Vector2Length(dir); // r2 isnt r^2 here
        dir = Vector2Normalize(dir);
        ef = Vector2Scale(dir, k / r2);
        U += arr[i].charge * arr[j].charge * Vector2Length(ef);
      }
      ke += 1 / 2.0 * arr[i].mass * Vector2LengthSqr(arr[i].v);
    }
    // dt = GetFrameTime();
    printf("PE:%f KE:%f TE:%f\n", U, ke, U + ke);
    // printf("v.x:%f v.y:%f \n", arr[noP - 2].v.x, arr[noP - 2].v.y);
    U = 0;
    ke = 0;
    EndDrawing();
  }
  // cleanup
  // unload our texture so it can be cleaned up
  // UnloadTexture(wabbit);

  // destroy the window and cleanup the OpenGL context
  free(arr);
  CloseWindow();
  return 0;
}
