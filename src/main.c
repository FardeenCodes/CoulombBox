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
#define NOTRAILPTS 100
#define WIDTHGRAPH 900

typedef struct {
  Vector2 pos[NOTRAILPTS];
  int head, tail, size;
} trailPts;
void InitTrailPts(trailPts *a) {
  a->head = 0;
  a->tail = 0;
  a->size = 0;
}

void addTrailPts(trailPts *a, Vector2 p) {
  a->pos[a->tail] = p;
  a->tail = (a->tail + 1) % NOTRAILPTS;
  if (a->size < NOTRAILPTS) {
    a->size++;
  } else
    a->head = (a->head + 1) % NOTRAILPTS;
}
void drawTrailPts(trailPts *a) {
  for (int i = 0; i < a->size; i++) {
    DrawCircleV(a->pos[(a->head + i) % NOTRAILPTS], 1, GRAY);
  }
}

typedef struct {
  float pe, ke, te;
} energies;
typedef struct {
  energies val[WIDTHGRAPH];
  int head, tail, size;
} graph;

void addEnergies(graph *a, float pe, float ke, float te) {
  a->val[a->tail].pe = pe;
  a->val[a->tail].ke = ke;
  a->val[a->tail].te = te;
  a->tail = (a->tail + 1) % WIDTHGRAPH;
  if (a->size < WIDTHGRAPH) {
    a->size++;
  } else
    a->head = (a->head + 1) % WIDTHGRAPH;
}
float grphBalance = 1;
void drawGraph(graph *a) {
  DrawLineEx((Vector2){WIDTH, 50.0f}, (Vector2){WIDTH, HEIGHT - 50.0f}, 4,
             GRAY);
  DrawLineEx((Vector2){WIDTH, 50.0f}, (Vector2){WIDTH + 10, 50.0f + 10}, 4,
             GRAY);
  DrawLineEx((Vector2){WIDTH, HEIGHT - 50.0f},
             (Vector2){WIDTH + 10, HEIGHT - (50.0f + 10)}, 4, GRAY);
  DrawText("TE:\nKE:\nPE:", WIDTH + 5, HEIGHT - 25 * 3, 25, WHITE);
  DrawLineEx((Vector2){WIDTH + MeasureText("TE:\nKE:\nPE:", 25) + 10,
                       HEIGHT - 75 + 25 / 2.0},
             (Vector2){WIDTH + MeasureText("TE:\nKE:\nPE:", 25) + 30,
                       HEIGHT - 75 + 25 / 2.0},
             5, BLUE);
  DrawLineEx((Vector2){WIDTH + MeasureText("TE:\nKE:\nPE:", 25) + 10,
                       HEIGHT - 75 + 25 / 2.0 * 3},
             (Vector2){WIDTH + MeasureText("TE:\nKE:\nPE:", 25) + 30,
                       HEIGHT - 75 + 25 / 2.0 * 3},
             5, RED);
  DrawLineEx((Vector2){WIDTH + MeasureText("TE:\nKE:\nPE:", 25) + 10,
                       HEIGHT - 75 + 25 / 2.0 * 5},
             (Vector2){WIDTH + MeasureText("TE:\nKE:\nPE:", 25) + 30,
                       HEIGHT - 75 + 25 / 2.0 * 5},
             5, GREEN);

  // graph axis
  DrawLineEx((Vector2){WIDTH, HEIGHT / 2},
             (Vector2){WIDTH + WIDTHGRAPH, HEIGHT / 2}, 2, GRAY);
  for (int i = 0; i < a->size; i++) {
    DrawPixel(
        i + WIDTH,
        (HEIGHT / 2 - grphBalance * (a->val[(a->head + i) % WIDTHGRAPH].pe)),
        GREEN);
    DrawPixel(
        i + WIDTH,
        (HEIGHT / 2 - grphBalance * (a->val[(a->head + i) % WIDTHGRAPH].ke)),
        RED);
    DrawPixel(
        i + WIDTH,
        (HEIGHT / 2 - grphBalance * (a->val[(a->head + i) % WIDTHGRAPH].te)),
        BLUE);
  }
}

typedef struct {
  Vector2 pos, v, acc;
  double charge;
  int mass;
  trailPts pts;
} chargeP;
chargeP *arr = NULL;
int noP = 0;

void drawCharge() {
  // draw box
  DrawLineEx((Vector2){WIDTH, 0}, (Vector2){WIDTH, HEIGHT}, 5, WHITE);
  DrawLineEx((Vector2){0, HEIGHT}, (Vector2){WIDTH, HEIGHT}, 4, WHITE);
  for (int i = 0; i < noP; i++) {
    DrawCircleV(arr[i].pos, RAD, arr[i].charge > 0 ? BLUE : RED);
    drawTrailPts(&arr[i].pts);
  }
}

// calculate EF arrows
void drawEFArrow() {
  float grid = 10.0f, len;
  Vector2 ef = {0, 0}, dir;
  Vector2 instPt = {0, 0};
  for (int i = 1; i < grid; i++) {
    for (int j = 1; j < grid; j++) {
      instPt.x = WIDTH / grid * i;
      instPt.y = HEIGHT / grid * j;
      // DrawCircleV(instPt, 10, BROWN);

      for (int m = 0; m < noP; m++) {
        dir = Vector2Subtract(instPt, arr[m].pos);
        ef = Vector2Add(
            ef, Vector2Scale(Vector2Normalize(dir),
                             k * arr[m].charge / Vector2LengthSqr(dir)));
      }
      len = 5 * Vector2Length(ef);
      len = len < WIDTH / (grid * 2) ? len : WIDTH / (grid * 2);
      // DrawLineV(Vector2Add(instPt, Vector2Scale(Vector2Normalize(ef), -len)),
      // Vector2Add(instPt, Vector2Scale(Vector2Normalize(ef), len)),
      // WHITE);
      DrawLineV(instPt,
                Vector2Add(instPt, Vector2Scale(Vector2Normalize(ef), len)),
                WHITE);

      // DrawCircleV(Vector2Add(instPt, Vector2Scale(Vector2Normalize(ef),
      // len)), 3, WHITE);
      DrawTriangle(
          Vector2Add(instPt, Vector2Scale(Vector2Normalize(ef), len + 1)),
          Vector2Add(Vector2Add(instPt, Vector2Scale(Vector2Normalize(ef),
                                                     3 * len / 4)),
                     Vector2Scale(Vector2Normalize((Vector2){ef.y, -ef.x}), 5)),
          Vector2Add(
              Vector2Add(instPt,
                         Vector2Scale(Vector2Normalize(ef), 3 * len / 4)),
              Vector2Scale(Vector2Normalize((Vector2){ef.y, -ef.x}), -5)),

          WHITE);
      // DrawText(TextFormat("EF.x:.00%f\n y:.00%f\n", i, j, ef.x, ef.y),
      // instPt.x, instPt.y + 10, 14, WHITE);
      // printf("(i,j)EF.x:%f y:%f\n", i, j, ef.x, ef.y);
      ef = (Vector2){0, 0};
    }
  }
}

int main() {
  // printf("hi");
  graph g;
  g.head = 0;
  g.tail = 0;
  g.size = 0;

  // Tell the window to use vsync and work on high DPI displays
  // SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  // Create the window and OpenGL context
  InitWindow(WIDTH + WIDTHGRAPH, HEIGHT, "Charge Particles Simulation");
  chargeP *tmp = NULL;
  chargeP tmpCharge;
  Vector2 ef = {0, 0}, dir, acc, startPos, vrel, vadd;
  float r2, u1, u2, U = 0, ke = 0, ovrLap;
  bool invalidP = false;

  SetTargetFPS(60);
  float dt, dU;                // game loop
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
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      tmpCharge.pos = startPos;
      tmpCharge.charge = +1;
      tmpCharge.mass = +1;
      tmpCharge.acc = (Vector2){0, 0};
      tmpCharge.v = Vector2Subtract(GetMousePosition(), startPos);

      InitTrailPts(&tmpCharge.pts);

      for (int i = 0; i < noP; i++) {

        if (CheckCollisionCircles(tmpCharge.pos, RAD, arr[i].pos, RAD)) {
          invalidP = true;
        }
      }
      if (tmpCharge.pos.x - RAD < 0 || tmpCharge.pos.x + RAD > WIDTH ||
          tmpCharge.pos.y - RAD < 0 || tmpCharge.pos.y + RAD > HEIGHT) {
        invalidP = true;
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
    } else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
      tmpCharge.pos = startPos;
      tmpCharge.charge = -1;
      tmpCharge.mass = +1;
      tmpCharge.acc = (Vector2){0, 0};
      tmpCharge.v = Vector2Subtract(GetMousePosition(), startPos);
      InitTrailPts(&tmpCharge.pts);

      if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        tmpCharge.v = Vector2Subtract(GetMousePosition(), startPos);
      }
      for (int i = 0; i < noP; i++) {

        if (CheckCollisionCircles(tmpCharge.pos, RAD, arr[i].pos, RAD)) {
          invalidP = true;
        }
      }

      if (tmpCharge.pos.x - RAD < 0 || tmpCharge.pos.x + RAD > WIDTH ||
          tmpCharge.pos.y - RAD < 0 || tmpCharge.pos.y + RAD > HEIGHT) {
        invalidP = true;
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
    invalidP = false;

    for (int i = 0; i < noP; i++) {
      // halfstep velocity
      // arr[i].v = Vector2Add(arr[i].v, Vector2Scale(arr[i].acc, 1 / 2.0 *
      // dt)); pos Update
      arr[i].pos = Vector2Add(
          arr[i].pos, Vector2Add(Vector2Scale(arr[i].v, dt),
                                 Vector2Scale(arr[i].acc, 1 / 2.0 * dt * dt)));
    }

    for (int i = 0; i < noP; i++) {
      ef = (Vector2){0, 0};
      for (int j = 0; j < noP; j++) {
        if (i == j) // no E on itself
          continue;
        dir = Vector2Subtract(arr[i].pos, arr[j].pos);
        r2 = Vector2LengthSqr(dir);
        dir = Vector2Normalize(dir);
        ef = Vector2Add(
            ef, Vector2Scale(dir, k * arr[i].charge * arr[j].charge / r2));
      }
      acc = Vector2Scale(ef, 1.0 / arr[i].mass);
      // acc = ef;
      arr[i].v = Vector2Add(arr[i].v, Vector2Scale(arr[i].acc, dt / 2.0));
      arr[i].acc = acc;
      arr[i].v = Vector2Add(arr[i].v, Vector2Scale(arr[i].acc, dt / 2.0));
    }
    for (int i = 0; i < noP; i++) {

      // wall collide[CORRECT]
      if (arr[i].pos.x - RAD <= 0)
        // if (arr[i].pos.x + arr[i].v.x - RAD <= 0){
        arr[i].v.x *= -1;
      else if (arr[i].pos.x + RAD >= WIDTH) {
        arr[i].v.x *= -1;
        // arr[i].pos.x = WIDTH - RAD; why did i even write this
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
            printf("collision\n");

            arr[i].v = Vector2Add(arr[i].v, Vector2Scale(dir, u2 - u1));
            arr[j].v = Vector2Add(arr[j].v, Vector2Scale(dir, u1 - u2));

            // Remove overlap;THIS IS THE CAUSE OF THAT UNEVEM TE
            U = 0;

            for (int m = 0; m < noP; m++) {
              for (int n = m + 1; n < noP; n++) {
                dir = Vector2Subtract(arr[m].pos, arr[n].pos);
                r2 = Vector2Length(dir); // r2 isnt r^2 here
                dir = Vector2Normalize(dir);
                ef = Vector2Scale(dir, k / r2);
                U += arr[n].charge * arr[m].charge * Vector2Length(ef);
              }
            }

            ovrLap = 2 * RAD -
                     (Vector2Length(Vector2Subtract(arr[i].pos, arr[j].pos)));
            if (ovrLap > 0) {
              dir = Vector2Normalize(
                  Vector2Subtract(arr[i].pos, arr[j].pos)); // pointing toward i
              arr[i].pos =
                  Vector2Add(arr[i].pos, Vector2Scale(dir, ovrLap / 2.0));
              arr[j].pos =
                  Vector2Add(arr[j].pos, Vector2Scale(dir, -ovrLap / 2.0));
              // BALANCING KE for sudden PE change
              dU = 0;
              for (int m = 0; m < noP; m++) {
                for (int n = m + 1; n < noP; n++) {
                  dir = Vector2Subtract(arr[m].pos, arr[n].pos);
                  r2 = Vector2Length(dir); // r2 isnt r^2 here
                  dir = Vector2Normalize(dir);
                  ef = Vector2Scale(dir, k / r2);
                  dU += arr[n].charge * arr[m].charge * Vector2Length(ef);
                }
              }
              dU = dU - U;
              vrel = Vector2Subtract(arr[i].v, arr[j].v);
              vadd = Vector2Add(arr[i].v, arr[j].v);
              // u2 = (-(2 * dU / arr[j].mass) + (Vector2LengthSqr(arr[j].v) +
              // (Vector2LengthSqr(arr[i].v))));
              // arr[j].v =
              // Vector2Scale(Vector2Normalize(arr[j].v),
              // u2);
              if (Vector2LengthSqr(vrel) - 4 * dU / arr[i].mass > 0) {

                arr[i].v =
                    Vector2Add(Vector2Scale(vadd, 1 / 2.0),
                               Vector2Scale(Vector2Normalize(vrel),
                                            sqrtf(Vector2LengthSqr(vrel) -
                                                  4 * dU / arr[i].mass) *
                                                1 / 2.0));
                arr[j].v =
                    Vector2Add(Vector2Scale(vadd, 1 / 2.0),
                               Vector2Scale(Vector2Normalize(vrel),
                                            sqrtf(Vector2LengthSqr(vrel) -
                                                  4 * dU / arr[i].mass) *
                                                -1 / 2.0));
              }
            }
          }
        }
      }
      addTrailPts(&arr[i].pts, arr[i].pos);
      // looks better except when collison happens
      //  addTrailPts(&arr[i].pts,
      //              Vector2Add(arr[i].pos,
      //                         Vector2Scale(Vector2Normalize(arr[i].v),
      //                         -RAD)));
      //  arr[i].pos = Vector2Add(arr[i].pos, Vector2Scale(arr[i].v, dt));
    }
    // // for U calc
    U = 0, ke = 0;
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

    printf("PE:%f KE:%f TE:%f\n", grphBalance * U, grphBalance * ke,
           grphBalance * (U + ke));
    addEnergies(&g, U, ke, U + ke);
    float max = fabsf(U) > fabsf(ke) ? U : ke;
    max = fabsf(max) > fabsf(U + ke) ? max : (U + ke);

    // looks better but shifts graph every frame
    grphBalance = (HEIGHT / 2 - 50.0f) / (max != 0 ? fabsf(max) : 1.0f);

    // while (fabsf(HEIGHT / 2 - grphBalance * max) > HEIGHT / 2 + 100) {
    //   grphBalance *= 0.1;
    // }
    U = 0;
    ke = 0;
    drawCharge();
    drawEFArrow();
    drawGraph(&g);
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
