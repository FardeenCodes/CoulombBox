/*//TODO:fix acc contri in wall collision or remove it
 // NOTE::encountered an issue with .didcollide  where this loops new
colliding particles interfered with last loops colliding particles pair

*/
// #include <std>
// //TODO :: ADD EVENT BASED COLLISION DETECTION FOR wall too (by adding one
// more bool var wall collision)
// NOTE: still fails and overlap happens when adding more particle which throws
// off the predicted first collision and time taken for it (dtoffset)
// for ex: when adding a lot of particles too close especially when particles ar
// already colliding
// or a particles just in path of a very fast moving particle
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
#define DT 1 / 60.0f

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
  bool didCollide, wallCollide; // test
  trailPts pts;
} chargeP;
chargeP *arr = NULL;
int noP = 0;

void drawCharge(int focusP) {
  // draw box
  DrawLineEx((Vector2){WIDTH, 0}, (Vector2){WIDTH, HEIGHT}, 5, WHITE);
  DrawLineEx((Vector2){0, HEIGHT}, (Vector2){WIDTH, HEIGHT}, 4, WHITE);
  for (int i = 0; i < noP; i++) {
    DrawCircleV(arr[i].pos, RAD, arr[i].charge > 0 ? BLUE : RED);
    if (focusP == i)
      DrawCircleLinesEx(arr[i].pos, RAD - RAD / 5, RAD / 5, GREEN);
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
typedef struct {
  float momx, x;
} phase;

typedef struct {
  Vector2 pos[900]; //(x,momx)
  int head, tail, size;
} P2;
void InitP2(P2 *a) {
  a->head = 0;
  a->tail = 0;
  a->size = 0;
}

void addP2(P2 *a, Vector2 p) {
  a->pos[a->tail] = p;
  // a->pos[a->tail].x += WIDTH;
  a->tail = (a->tail + 1) % 900;
  if (a->size < 900) {
    a->size++;
  } else
    a->head = (a->head + 1) % 900;
}
void drawP2(P2 *a) {
  DrawLineEx((Vector2){WIDTH, 50.0f}, (Vector2){WIDTH, HEIGHT - 50.0f}, 4,
             GRAY);
  DrawLineEx((Vector2){WIDTH, 50.0f}, (Vector2){WIDTH + 10, 50.0f + 10}, 4,
             GRAY);
  DrawLineEx((Vector2){WIDTH, HEIGHT - 50.0f},
             (Vector2){WIDTH + 10, HEIGHT - (50.0f + 10)}, 4, GRAY);
  DrawLineEx((Vector2){WIDTH, HEIGHT / 2}, (Vector2){WIDTH + WIDTH, HEIGHT / 2},
             4, GRAY);

  // DrawLineEx((Vector2){WIDTH, 0}, (Vector2){WIDTH, HEIGHT}, 5, WHITE);
  // DrawLineEx((Vector2){0, HEIGHT}, (Vector2){WIDTH, HEIGHT}, 4, WHITE);
  // DrawLineEx((Vector2){WIDTH,HEIGHT/2}, (Vector2){WIDTH+WIDTH,HEIGHT/2} , 2,
  // WHITE)
  for (int i = 0; i < a->size; i++) {
    DrawCircle(3 / 2.0 * WIDTH + a->pos[(a->head + i) % 900].x,
               HEIGHT / 2 - grphBalance * a->pos[(a->head + i) % 900].y, 1,
               MAGENTA);
    printf("  x:%f  mom:%f     \n", a->pos[(a->head + i) % 900].x,
           a->pos[(a->head + i) % 900].y);
  }
}

int main() {

  // printf("hi");
  graph g;
  g.head = 0;
  g.tail = 0;
  g.size = 0;
  int panel = 0, focusP = -1;
  bool isStopped = false, toggleEF = true;

  // Tell the window to use vsync and work on high DPI displays
  // SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  // Create the window and OpenGL context
  P2 phaseofp2;
  InitP2(&phaseofp2);
  InitWindow(WIDTH + WIDTHGRAPH + 900, HEIGHT, "Charge Particles Simulation");
  chargeP *tmp = NULL;
  chargeP tmpCharge;
  Vector2 ef = {0, 0}, dir, acc, startPos, vrel, vadd;
  float var, r2, u1, u2, U = 0, ke = 0, ovrLap, TE = 0;
  bool invalidP = false, wcollWillhappen = false, pcollWillhappen = false;
  int collidingP1 = -1, collidingP2 = -1;
  int CollidedP[2] = {-1, -1};

  SetTargetFPS(60);
  float dt, dU, dtoffset = 0.0f; // game loop
  while (!WindowShouldClose())   // run the loop until the user presses ESCAPE
                                 // or presses the Close button on the window

  {
    dt = 1 / 60.0f - dtoffset;
    dtoffset = 0;
    // dt = 1 / 60.0f;
    // dt = GetFrameTime();
    // drawing
    BeginDrawing();
    // startPos = (Vector2){0, 0};
    invalidP = false;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
      startPos = GetMousePosition();
    else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
      startPos = GetMousePosition();
    if (IsKeyPressed(KEY_P)) { // P toogles side panel
      panel = (panel + 1) % 3;
    }
    if (IsKeyPressed(KEY_F)) {
      startPos = GetMousePosition();
      for (int i = 0; i < noP; i++) {
        if (CheckCollisionPointCircle(startPos, arr[i].pos, RAD)) {
          focusP = i;
        }
      }
    }

    if (IsKeyPressed(KEY_SPACE)) {
      printf("space");
      isStopped = !isStopped;
    }
    if (IsKeyPressed(KEY_E)) {
      toggleEF = !toggleEF;
    }

    if (isStopped) {
      EndDrawing();
      continue;
    }

    ClearBackground(BLACK);

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      tmpCharge.didCollide = false;
      tmpCharge.wallCollide = false;
      tmpCharge.pos = startPos;
      tmpCharge.charge = +1;
      tmpCharge.mass = +1;
      tmpCharge.acc = (Vector2){0, 0};
      tmpCharge.v = Vector2Subtract(GetMousePosition(), startPos);

      InitTrailPts(&tmpCharge.pts);

      for (int i = 0; i < noP; i++) {

        if (CheckCollisionCircles(tmpCharge.pos, RAD, arr[i].pos, RAD)) {
          invalidP = true;
          printf("invalidP");
        }
      }
      if (tmpCharge.pos.x - RAD < 0 || tmpCharge.pos.x + RAD > WIDTH ||
          tmpCharge.pos.y - RAD < 0 || tmpCharge.pos.y + RAD > HEIGHT) {
        invalidP = true;
        printf("invalidP");
      }
      if (!(invalidP)) {
        if (noP == 1 && IsKeyDown(KEY_C)) {
          Vector2 d = Vector2Subtract(tmpCharge.pos, arr[0].pos);
          float r = Vector2Length(d);

          Vector2 tangent = {d.y / r, -d.x / r};

          float vRel =
              sqrtf(k / r * (1.0f / arr[0].mass + 1.0f / tmpCharge.mass));

          tmpCharge.v = Vector2Add(arr[0].v, Vector2Scale(tangent, vRel));
        }

        chargeP *tmp = (chargeP *)realloc(arr, (noP + 1) * sizeof(chargeP));
        if (tmp == NULL) {
          continue;
        }
        noP++;
        arr = tmp;
        arr[noP - 1] = tmpCharge;
        TE = U + ke;
        // printf("U+ke:%f",TE);
        U = 0, ke = 0;
        for (int i = 0; i < noP - 1; i++) {
          dir = Vector2Subtract(arr[i].pos, tmpCharge.pos);
          r2 = Vector2Length(dir); // r2 isnt r^2 here
          dir = Vector2Normalize(dir);
          ef = Vector2Scale(dir, k / r2);
          U += arr[i].charge * tmpCharge.charge * Vector2Length(ef);
        }
        TE += 1 / 2.0 * tmpCharge.mass * Vector2LengthSqr(tmpCharge.v) + U;
        printf("E(0):%f", TE);
        U = 0;

        // dtoffset = 1/60.0f -dt;

        goto Collision;
        // free(tmp);NEVER FREE TMP ITS POINTING TO ARR
      }
    } else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
      tmpCharge.didCollide = false;
      tmpCharge.wallCollide = false;
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
          printf("invalidP");
        }
      }

      if (tmpCharge.pos.x - RAD < 0 || tmpCharge.pos.x + RAD > WIDTH ||
          tmpCharge.pos.y - RAD < 0 || tmpCharge.pos.y + RAD > HEIGHT) {
        invalidP = true;
        printf("invalidP");
      }

      if (!(invalidP)) {
        if (noP == 1 && IsKeyDown(KEY_C)) {

          Vector2 d = Vector2Subtract(tmpCharge.pos, arr[0].pos);
          float r = Vector2Length(d);

          Vector2 tangent = {d.y / r, -d.x / r};

          float vRel =
              sqrtf(k / r * (1.0f / arr[0].mass + 1.0f / tmpCharge.mass));

          tmpCharge.v = Vector2Add(arr[0].v, Vector2Scale(tangent, vRel));
        }

        chargeP *tmp = (chargeP *)realloc(arr, (noP + 1) * sizeof(chargeP));
        if (tmp == NULL) {
          continue;
        }
        noP++;
        arr = tmp;
        arr[noP - 1] = tmpCharge;
        TE += U + ke;
        U = 0, ke = 0;
        for (int i = 0; i < noP - 1; i++) {
          dir = Vector2Subtract(arr[i].pos, tmpCharge.pos);
          r2 = Vector2Length(dir); // r2 isnt r^2 here
          dir = Vector2Normalize(dir);
          ef = Vector2Scale(dir, k / r2);
          U += arr[i].charge * tmpCharge.charge * Vector2Length(ef);
        }
        TE += 1 / 2.0 * tmpCharge.mass * Vector2LengthSqr(tmpCharge.v) + U;
        U = 0;

        goto Collision;

        // free(tmp);NEVER FREE TMP ITS POINTING TO ARR
      }
    }

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
        // test adding padding to make a max effect of force
        r2 = Vector2LengthSqr(dir) + 0.000001f;
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
  Collision:
    for (int i = 0; i < noP; i++) {

      // wall collide[CORRECT]
      // // handling
      //
      if (CollidedP[1] == -5 && arr[i].wallCollide) {
        printf("WALL COLLISION HANDLED\n");
        // if (arr[i].pos.x + arr[i].v.x - RAD <= 0){
        arr[i].v.x *= -1;

        arr[i].wallCollide = false;
      }
      // NOTE:HAD TO remove exact checking value of distance checking due to
      // numerical/floating pt errors
      //
      //  } else if (CollidedP[1] == -3 && arr[i].wallCollide) {
      //    printf("WALL COLLISION HANDLED\n");
      //    arr[i].v.x *= -1;
      //    arr[i].wallCollide = false;
      //  }
      //  arr[i].pos.x = WIDTH - RAD; why did i even write this

      if (CollidedP[1] == -2 && arr[i].wallCollide) {
        printf("WALL COLLISION HANDLED\n");
        arr[i].v.y *= -1;
        arr[i].wallCollide = false;
      }
      // detecting
      if ((arr[i].pos.x + arr[i].v.x * DT + 0.5f * arr[i].acc.x * DT * DT) -
                  RAD <=
              0 &&
          arr[i].v.x < 0) {
        double tcoll = ((arr[i].v.x * arr[i].v.x) +
                        (2 * arr[i].acc.x * ((RAD - arr[i].pos.x))));
        u2 = arr[i].acc.x;
        u1 = arr[i].v.x;
        if (fabsf(u2) > 0.0f && !isnan(tcoll)) {

          if (tcoll >= 0.0 && isfinite(tcoll)) {
            tcoll = sqrt(tcoll);
            double v_rootD = -0.5 * (u1 + copysign(tcoll, u1));
            double root1, root2;
            // q is needed bcz float was approx v - very smal root(D) as v
            if (v_rootD != 0.0) {
              root1 = v_rootD / (0.5 * u2);
              root2 = (WIDTH - (arr[i].pos.x + RAD)) / v_rootD;

              // Check each root independently for 0 < root <= DT.
            } else {
              root1 = -1;
              root2 = -1;
            }

            if (root1 < root2 && root1 > 0 && root1 <= DT) {
              // printf("if else a\n");
              var = DT - root1;
            } else if (root1 > root2 && root2 > 0 && root2 <= DT) {
              // printf("if if a  tcoll:%f", tcoll);
              var = DT - root2;
            } else if (root1 > 0 && root1 <= DT) {

              var = DT - root1;
            } else if (root2 > 0 && root2 <= DT) {
              var = DT - root2;
            } else {
              // already crossed wall
              printf("no valid root");
              var = 0; // for preventing it from hindering other
              printf(" acc:%f,v:%f,x:%f\n", u2, u1, arr[i].pos.y);

              // var = DT - (((-u1 - tcoll) / u2) > 0 ? ((-u1 - tcoll) / u2)
              // : ((-u1 + tcoll) / u2));
              //   // when both are -ve
              arr[i].v.x *= -1;
            }
          }
        } else {
          printf("else a\n");
          var = DT - fabsf((arr[i].pos.x - RAD) / arr[i].v.x);
        }
        if (var < 0)
          printf("-ve var\n");
        if (var > DT) {
          printf("how tf");
        }
        // var = var > DT ? 0 : var; // comment out to check overlap happens
        if (var > dtoffset /*|| dtoffset == 0.0f*/) {
          printf(" acc:%f,v:%f,y:%f\n", u2, u1, arr[i].pos.x);
          if (wcollWillhappen) {
            arr[collidingP1].wallCollide = false;
          } else if (pcollWillhappen) {
            arr[collidingP1].didCollide = false;
            arr[collidingP2].didCollide = false;
          }
          wcollWillhappen = true;
          pcollWillhappen = false;
          collidingP1 = i;
          collidingP2 = -5; // left&right wall

          arr[i].wallCollide = true;
          dtoffset = var;
        }
      }

      // if (arr[i].pos.x + arr[i].v.x * DT + 0.5f * arr[i].acc.x * DT * DT -
      //             RAD <=
      //         0 &&
      //     arr[i].v.x < 0) {
      //   var = 1 / 60.0f - fabsf((arr[i].pos.x - RAD) / arr[i].v.x);
      //   if (var > dtoffset || dtoffset == 0.0f) {
      //     if (wcollWillhappen) {
      //       arr[collidingP1].wallCollide = false;
      //     } else if (pcollWillhappen) {
      //       arr[collidingP1].didCollide = false;
      //       arr[collidingP2].didCollide = false;
      //     }
      //     wcollWillhappen = true;
      //     pcollWillhappen = false;
      //     collidingP1 = i;
      //     collidingP2 = -5; //-5=right &left wall
      //     arr[i].wallCollide = true;
      //     dtoffset = var;
      //   }
      // }
      // if (arr[i].pos.x + arr[i].v.x - RAD <= 0){
      else if ((arr[i].pos.x + arr[i].v.x * DT +
                0.5f * arr[i].acc.x * DT * DT) +
                       RAD >=
                   WIDTH &&
               arr[i].v.x > 0) {
        double tcoll = ((arr[i].v.x * arr[i].v.x) +
                        (2 * arr[i].acc.x * (WIDTH - (RAD + arr[i].pos.x))));
        u2 = arr[i].acc.x;
        u1 = arr[i].v.x;
        if (fabsf(u2) > 0.0f && !isnan(tcoll)) {

          if (tcoll >= 0.0 && isfinite(tcoll)) {
            tcoll = sqrt(tcoll);
            double v_rootD = -0.5 * (u1 + copysign(tcoll, u1));
            double root1, root2;
            // q is needed bcz float was approx v - very smal root(D) as v
            if (v_rootD != 0.0) {
              root1 = v_rootD / (0.5 * u2);
              root2 = (WIDTH - (arr[i].pos.x + RAD)) / v_rootD;

              // Check each root independently for 0 < root <= DT.
            } else {
              root1 = -1;
              root2 = -1;
            }

            if (root1 < root2 && root1 > 0 && root1 <= DT) {
              // printf("if else a\n");
              var = DT - root1;
            } else if (root1 > root2 && root2 > 0 && root2 <= DT) {
              // printf("if if a  tcoll:%f", tcoll);
              var = DT - root2;
            } else if (root1 > 0 && root1 <= DT) {

              var = DT - root1;
            } else if (root2 > 0 && root2 <= DT) {
              var = DT - root2;
            } else {
              // already crossed wall
              printf("no valid root");
              var = 0; // for preventing it from hindering other
              printf(" acc:%f,v:%f,x:%f\n", u2, u1, arr[i].pos.x);

              // var = DT - (((-u1 - tcoll) / u2) > 0 ? ((-u1 - tcoll) / u2)
              // : ((-u1 + tcoll) / u2));
              //   // when both are -ve
              arr[i].v.x *= -1;
            }
          }
        } else {
          printf("else a\n");
          var = DT - fabsf((WIDTH - (arr[i].pos.x + RAD)) / arr[i].v.x);
        }
        if (var < 0)
          printf("-ve var\n");
        if (var > DT) {
          printf("how tf");
        }
        // var = var > DT ? 0 : var; // comment out to check overlap happens
        if (var > dtoffset /*|| dtoffset == 0.0f*/) {
          printf(" acc:%f,v:%f,y:%f\n", u2, u1, arr[i].pos.y);
          if (wcollWillhappen) {
            arr[collidingP1].wallCollide = false;
          } else if (pcollWillhappen) {
            arr[collidingP1].didCollide = false;
            arr[collidingP2].didCollide = false;
          }
          wcollWillhappen = true;
          pcollWillhappen = false;
          collidingP1 = i;
          collidingP2 = -5; // left&right wall

          arr[i].wallCollide = true;
          dtoffset = var;
        }
      }

      // else if (arr[i].pos.x + arr[i].v.x * DT + 0.5f * arr[i].acc.x * DT * DT
      // +
      //                  RAD >=
      //              WIDTH &&
      //          arr[i].v.x > 0) {
      //   var = 1 / 60.0f - fabsf((WIDTH - (arr[i].pos.x + RAD)) / arr[i].v.x);
      //   if (var > dtoffset || dtoffset == 0.0f) {
      //     if (wcollWillhappen) {
      //       arr[collidingP1].wallCollide = false;
      //     } else if (pcollWillhappen) {
      //       arr[collidingP1].didCollide = false;
      //       arr[collidingP2].didCollide = false;
      //     }
      //     wcollWillhappen = true;
      //     pcollWillhappen = false;
      //     collidingP1 = i;
      //     collidingP2 = -5; //-5=right&left wall
      //
      //     arr[i].wallCollide = true;
      //     dtoffset = var;
      //   }
      // }
      // arr[i].pos.x = WIDTH - RAD; why did i even write this

      // if (arr[i].pos.y + arr[i].v.y * DT + 0.5f * arr[i].acc.y * DT * DT +
      //             RAD >=
      //         HEIGHT &&
      //     arr[i].v.y > 0) {
      //   var = 1 / 60.0f - fabsf((HEIGHT - (arr[i].pos.y + RAD)) /
      //   arr[i].v.y); if (var > dtoffset || dtoffset == 0.0f) {
      //     if (wcollWillhappen) {
      //       arr[collidingP1].wallCollide = false;
      //     } else if (pcollWillhappen) {
      //       arr[collidingP1].didCollide = false;
      //       arr[collidingP2].didCollide = false;
      //     }
      //     wcollWillhappen = true;
      //     pcollWillhappen = false;
      //     collidingP1 = i;
      //     collidingP2 = -2; //-2=Upper &lower wall
      //
      //     arr[i].wallCollide = true;
      //     dtoffset = var;
      //   }
      // }
      if ((arr[i].pos.y + arr[i].v.y * DT + 0.5f * arr[i].acc.y * DT * DT) +
                  RAD >=
              HEIGHT &&
          arr[i].v.y > 0) {
        double tcoll = ((arr[i].v.y * arr[i].v.y) +
                        (2 * arr[i].acc.y * (HEIGHT - (RAD + arr[i].pos.y))));
        u2 = arr[i].acc.y;
        u1 = arr[i].v.y;
        if (fabsf(u2) > 0.0f && !isnan(tcoll)) {

          if (tcoll >= 0.0 && isfinite(tcoll)) {
            tcoll = sqrt(tcoll);
            double v_rootD = -0.5 * (u1 + copysign(tcoll, u1));
            double root1, root2;
            // q is needed bcz float was approx v - very smal root(D) as v
            if (v_rootD != 0.0) {
              root1 = v_rootD / (0.5 * u2);
              root2 = (HEIGHT - (arr[i].pos.y + RAD)) / v_rootD;

              // Check each root independently for 0 < root <= DT.
            } else {
              root1 = -1;
              root2 = -1;
            }

            if (root1 < root2 && root1 > 0 && root1 <= DT) {
              // printf("if else a\n");
              var = DT - root1;
            } else if (root1 > root2 && root2 > 0 && root2 <= DT) {
              // printf("if if a  tcoll:%f", tcoll);
              var = DT - root2;
            } else if (root1 > 0 && root1 <= DT) {

              var = DT - root1;
            } else if (root2 > 0 && root2 <= DT) {
              var = DT - root2;
            } else {
              // already crossed wall
              printf("no valid root");
              var = 0; // for preventing it from hindering other
              printf(" acc:%f,v:%f,y:%f\n", u2, u1, arr[i].pos.y);

              // var = DT - (((-u1 - tcoll) / u2) > 0 ? ((-u1 - tcoll) / u2)
              // : ((-u1 + tcoll) / u2));
              //   // when both are -ve
              arr[i].v.y *= -1;
            }
          }
        } else {
          printf("else a\n");
          var = DT - fabsf((HEIGHT - (arr[i].pos.y + RAD)) / arr[i].v.y);
        }
        if (var < 0)
          printf("-ve var\n");
        if (var > DT) {
          printf("how tf");
        }
        // var = var > DT ? 0 : var; // comment out to check overlap happens
        if (var > dtoffset /*|| dtoffset == 0.0f*/) {
          printf(" acc:%f,v:%f,y:%f\n", u2, u1, arr[i].pos.y);
          if (wcollWillhappen) {
            arr[collidingP1].wallCollide = false;
          } else if (pcollWillhappen) {
            arr[collidingP1].didCollide = false;
            arr[collidingP2].didCollide = false;
          }
          wcollWillhappen = true;
          pcollWillhappen = false;
          collidingP1 = i;
          collidingP2 = -2; //-2=upper&below wall

          arr[i].wallCollide = true;
          dtoffset = var;
        }
      }

      // else if (arr[i].pos.y + arr[i].v.y * DT +
      //                    0.5f * arr[i].acc.y * DT * DT - RAD <=
      //                0 &&
      //            arr[i].v.y < 0) {
      //   var = 1 / 60.0f - fabsf((arr[i].pos.y - RAD) / arr[i].v.y);
      //   if (var > dtoffset || dtoffset == 0.0f) {
      //     if (wcollWillhappen) {
      //       arr[collidingP1].wallCollide = false;
      //     } else if (pcollWillhappen) {
      //       arr[collidingP1].didCollide = false;
      //       arr[collidingP2].didCollide = false;
      //     }
      //     wcollWillhappen = true;
      //     pcollWillhappen = false;
      //     collidingP1 = i;
      //
      //     arr[i].wallCollide = true;
      //     dtoffset = var;
      //   }
      // }
      //
      // 0=1/2at^2+ut-S
      // s=rad
      // t=-u+-sqrt(u^2+2accS)/acc
      //
      else if ((arr[i].pos.y + arr[i].v.y * DT +
                0.5f * arr[i].acc.y * DT * DT) -
                       RAD <=
                   0 &&
               arr[i].v.y < 0) {
        double tcoll = ((arr[i].v.y * arr[i].v.y) +
                        (2 * arr[i].acc.y * (RAD - arr[i].pos.y)));
        u2 = arr[i].acc.y;
        u1 = arr[i].v.y;
        if (fabsf(u2) > 0.0f && !isnan(tcoll)) {

          if (tcoll >= 0.0 && isfinite(tcoll)) {
            tcoll = sqrt(tcoll);
            double v_rootD = -0.5 * (u1 + copysign(tcoll, u1));
            double root1, root2;
            // q is needed bcz float was approx v - very smal root(D) as v
            if (v_rootD != 0.0) {
              root1 = v_rootD / (0.5 * u2);
              root2 = (arr[i].pos.y - RAD) / v_rootD;

              // Check each root independently for 0 < root <= DT.
            } else {
              root1 = -1;
              root2 = -1;
            }

            if (root1 < root2 && root1 > 0 && root1 <= DT) {
              // printf("if else a\n");
              var = DT - root1;
            } else if (root1 > root2 && root2 > 0 && root2 <= DT) {
              // printf("if if a  tcoll:%f", tcoll);
              var = DT - root2;
            } else if (root1 > 0 && root1 <= DT) {

              var = DT - root1;
            } else if (root2 > 0 && root2 <= DT) {
              var = DT - root2;
            } else {
              // already crossed wall
              printf("no valid root");
              var = 0; // for preventing it from hindering other
              printf(" acc:%f,v:%f,y:%f\n", u2, u1, arr[i].pos.y);

              // var = DT - (((-u1 - tcoll) / u2) > 0 ? ((-u1 - tcoll) / u2)
              // : ((-u1 + tcoll) / u2));
              //   // when both are -ve
              arr[i].v.y *= -1;
            }
          }

          // if (((-u1 + tcoll) / u2) < (-u1 - tcoll) / u2 &&
          //     ((-u1 + tcoll) / u2) > 0 && ((-u1 + tcoll) / u2) <= DT) {
          //   // printf("if else a\n");
          //   var = DT - ((-u1 + tcoll) / u2);
          // } else if (((-u1 + tcoll) / u2) > (-u1 - tcoll) / u2 &&
          //            ((-u1 - tcoll) / u2) > 0 && ((-u1 - tcoll) / u2) <=
          //            DT)
          //            {
          //   // printf("if if a  tcoll:%f", tcoll);
          //   var = DT - ((-u1 - tcoll) / u2);
          // } else if (((-u1 + tcoll) / u2) > 0 && ((-u1 + tcoll) / u2) <=
          // DT)
          // {
          //
          //   var = DT - ((-u1 + tcoll) / u2);
          // } else if (((-u1 - tcoll) / u2) > 0 && ((-u1 - tcoll) / u2) <=
          // DT)
          // {
          //   var = DT - ((-u1 - tcoll) / u2);
          // } else {
          //   printf("why no valid root");
          //   var = DT; // for preventing it from hindering other
          //   printf(" acc:%f,v:%f,y:%f\n", u2, u1, arr[i].pos.y);
          //
          //   // var = DT - (((-u1 - tcoll) / u2) > 0 ? ((-u1 - tcoll) /
          //   u2)
          //   // : ((-u1 + tcoll) / u2));
          //   //   // when both are -ve
          //   // arr[i].v.y *= -1;}
          // }
        } else {
          printf("else a\n");
          var = DT - fabsf(((arr[i].pos.y - RAD)) / arr[i].v.y);
        }
        if (var < 0)
          printf("-ve var\n");
        if (var > DT) {
          printf("how tf");
        }
        // var = var > DT ? 0 : var; // comment out to check overlap happens
        if (var > dtoffset /*|| dtoffset == 0.0f*/) {
          printf(" acc:%f,v:%f,y:%f\n", u2, u1, arr[i].pos.y);
          if (wcollWillhappen) {
            arr[collidingP1].wallCollide = false;
          } else if (pcollWillhappen) {
            arr[collidingP1].didCollide = false;
            arr[collidingP2].didCollide = false;
          }
          wcollWillhappen = true;
          pcollWillhappen = false;
          collidingP1 = i;
          collidingP2 = -2; //-2=upper&below wall

          arr[i].wallCollide = true;
          dtoffset = var;
        }
      }
      //
      // collision among particles
      // thought: if collision is going to happen reduce dt to when it will
      // happen and sice multi particle make it the min as to handle the
      // first collision and when dist ==r*2 velocity exchange
      for (int j = i + 1; j < noP; j++) {
        // if (Vector2Length(Vector2Subtract(arr[i].pos, arr[j].pos)) ==
        // 2.0f * RAD) {

        // if (arr[j].didCollide && arr[i].didCollide) {
        // && Vector2LengthSqr(Vector2Subtract(arr[i].pos, arr[j].pos))
        // - 1.0f < 2.0 * RAD) {
        if (i == CollidedP[0] && j == CollidedP[1]) {
          printf("COLLISION HANDLED\n");
          dir = Vector2Normalize(Vector2Subtract(arr[i].pos, arr[j].pos));
          float vnBefore =
              Vector2DotProduct(Vector2Subtract(arr[i].v, arr[j].v), dir);

          // Your existing velocity updates go here.

          u1 = Vector2DotProduct(arr[i].v, dir);

          u2 = Vector2DotProduct(arr[j].v, dir);
          arr[i].v = Vector2Add(arr[i].v, Vector2Scale(dir, u2 - u1));
          arr[j].v = Vector2Add(arr[j].v, Vector2Scale(dir, u1 - u2));
          // after v
          float vnAfter =
              Vector2DotProduct(Vector2Subtract(arr[i].v, arr[j].v), dir);

          printf("pair (%d,%d), selected (%d,%d), "
                 "vn before: %f, vn after: %f\n",
                 i, j, collidingP1, collidingP2, vnBefore, vnAfter);

          arr[i].didCollide = false;
          arr[j].didCollide = false;
          CollidedP[0] = -1;
          CollidedP[1] = -1;
        }

        else if (CheckCollisionCircles(
                     Vector2Add(
                         arr[i].pos,
                         Vector2Add(Vector2Scale(arr[i].v, DT),
                                    Vector2Scale(arr[i].acc, 0.5 * DT * DT))),
                     RAD,
                     Vector2Add(
                         arr[j].pos,
                         Vector2Add(Vector2Scale(arr[j].v, DT),
                                    Vector2Scale(arr[j].acc, 0.5 * DT * DT))),
                     RAD) &&
                 Vector2DotProduct(Vector2Subtract(arr[i].pos, arr[j].pos),
                                   Vector2Subtract(arr[j].v, arr[i].v)) > 0) {
          vrel = Vector2Subtract(arr[i].v, arr[j].v);
          u1 = Vector2DotProduct(
              vrel, Vector2Normalize(Vector2Subtract(arr[j].pos, arr[i].pos)));
          // accrel
          u2 = (Vector2DotProduct(
              Vector2Subtract(arr[i].acc, arr[j].acc),
              Vector2Normalize(Vector2Subtract(arr[j].pos, arr[i].pos))));
          // ut+1/2acct^2-S=0
          // t=-u+-sqrt(u^2+2accS)/acc
          float tcoll = sqrtf(
              powf(Vector2DotProduct(vrel, Vector2Normalize(Vector2Subtract(
                                               arr[j].pos, arr[i].pos))),
                   2) +
              2 *
                  Vector2DotProduct(Vector2Subtract(arr[i].acc, arr[j].acc),
                                    Vector2Normalize(Vector2Subtract(
                                        arr[j].pos, arr[i].pos))) *
                  (Vector2Length(Vector2Subtract(arr[i].pos, arr[j].pos)) -
                   (2 * RAD)));
          if ((-u1 + tcoll) / u2 > (-u1 - tcoll) / u2 &&
              ((-u1 - tcoll) / u2) > 0) {
            var = DT - ((-u1 - tcoll) / u2);
          } else if ((-u1 + tcoll) / u2 < (-u1 - tcoll) / u2 &&
                     ((-u1 + tcoll) / u2) > 0)
            var = DT - ((-u1 + tcoll) / u2);
          else if ((-u1 + tcoll) / u2 > 0 && (-u1 + tcoll) / u2 < DT) {
            var = DT - (-u1 + tcoll) / u2;
          } else if ((-u1 - tcoll) / u2 > 0 && (-u1 - tcoll) / u2 < DT) {
            var = DT - (-u1 - tcoll) / u2;
            // var = DT - 0;
          } else {
            // UNHANDLED OVERLAP
            var = DT;

            printf("why\n");
          }
          // var = DT - ((Vector2Length(Vector2Subtract(arr[i].pos,
          // arr[j].pos))
          // -
          //              RAD * 2) /
          //             (Vector2DotProduct(vrel,
          //             Vector2Normalize(Vector2Subtract(
          //                                          arr[j].pos,
          //                                          arr[i].pos)))));
          // THINK WHAT TO DO WITH OVERLAPPING PARTICLES(TOO HIGH SPEED)
          //
          // if (var > DT)
          //   printf("overlap between partocles happened\n");
          // var = var > DT ? 0 : var; // comment out to check overlap
          // happens or not dt -ve means ovrlap did happen
          if (var > dtoffset /*|| dtoffset == 0.0f*/) {
            printf("COLLISION DETECTED\n");
            printf("vrel:%f | distance: %f | var: %f | dt next: %f\n",
                   (Vector2DotProduct(vrel, Vector2Normalize(Vector2Subtract(
                                                arr[j].pos, arr[i].pos)))),
                   Vector2Length(Vector2Subtract(arr[i].pos, arr[j].pos)), var,
                   DT - var);
            if (wcollWillhappen) {
              arr[collidingP1].wallCollide = false;
            } else if (pcollWillhappen) {
              arr[collidingP1].didCollide = false;
              arr[collidingP2].didCollide = false;
            }
            pcollWillhappen = true;
            wcollWillhappen = false;
            collidingP1 = i;
            collidingP2 = j;

            arr[i].didCollide = true;
            arr[j].didCollide = true;
            dtoffset = var;
          }

          // if (CheckCollisionCircles(arr[i].pos, RAD, arr[j].pos, RAD) &&
          // i
          // != j) {
          //   dir = Vector2Normalize(
          //       Vector2Subtract(arr[i].pos, arr[j].pos)); // pointing
          //       toward
          //       i
          // SHOULD DIR VECTOR BE WRT PREDICTED POS OR ACTUAL POS
          // dir = Vector2Normalize(Vector2Subtract(
          //     Vector2Add(arr[i].pos, Vector2Scale(arr[i].v, dt)),
          //     Vector2Add(arr[j].pos, Vector2Scale(arr[j].v, dt))));
          // arr[i].v = Vector2Add(arr[i].v, Vector2Scale(dir, u2 - u1));
          // arr[j].v = Vector2Add(arr[j].v, Vector2Scale(dir, u1 - u2));

          // Remove overlap;THIS IS THE CAUSE OF THAT UNEVEM TE
          // U = 0;
          //
          // for (int m = 0; m < noP; m++) {
          //   for (int n = m + 1; n < noP; n++) {
          //     dir = Vector2Subtract(arr[m].pos, arr[n].pos);
          //     r2 = Vector2Length(dir); // r2 isnt r^2 here
          //     dir = Vector2Normalize(dir);
          //     ef = Vector2Scale(dir, k / r2);
          //     U += arr[n].charge * arr[m].charge * Vector2Length(ef);
          //   }
          // }
          //
          // ovrLap = 2 * RAD -
          //          (Vector2Length(Vector2Subtract(arr[i].pos,
          //          arr[j].pos)));
          // if (ovrLap > 0) {
          //   dir = Vector2Normalize(
          //       Vector2Subtract(arr[i].pos, arr[j].pos)); // pointing
          //       toward i
          //   arr[i].pos =
          //       Vector2Add(arr[i].pos, Vector2Scale(dir, ovrLap / 2.0));
          //   arr[j].pos =
          //       Vector2Add(arr[j].pos, Vector2Scale(dir, -ovrLap / 2.0));
          //   // BALANCING KE for sudden PE change
          //   dU = 0;
          //   for (int m = 0; m < noP; m++) {
          //     for (int n = m + 1; n < noP; n++) {
          //       dir = Vector2Subtract(arr[m].pos, arr[n].pos);
          //       r2 = Vector2Length(dir); // r2 isnt r^2 here
          //       dir = Vector2Normalize(dir);
          //       ef = Vector2Scale(dir, k / r2);
          //       dU += arr[n].charge * arr[m].charge * Vector2Length(ef);
          //     }
          //   }
          //   dU = dU - U;
          //   vrel = Vector2Subtract(arr[i].v, arr[j].v);
          //   vadd = Vector2Add(arr[i].v, arr[j].v);
          //   // u2 = (-(2 * dU / arr[j].mass) +
          //   (Vector2LengthSqr(arr[j].v) +
          //   // (Vector2LengthSqr(arr[i].v))));
          //   // arr[j].v =
          //   // Vector2Scale(Vector2Normalize(arr[j].v),
          //   // u2);
          //   if (Vector2LengthSqr(vrel) - 4 * dU / arr[i].mass > 0) {
          //
          //     arr[i].v =
          //         Vector2Add(Vector2Scale(vadd, 1 / 2.0),
          //                    Vector2Scale(Vector2Normalize(vrel),
          //                                 sqrtf(Vector2LengthSqr(vrel) -
          //                                       4 * dU / arr[i].mass) *
          //                                     1 / 2.0));
          //     arr[j].v =
          //         Vector2Add(Vector2Scale(vadd, 1 / 2.0),
          //                    Vector2Scale(Vector2Normalize(vrel),
          //                                 sqrtf(Vector2LengthSqr(vrel) -
          //                                       4 * dU / arr[i].mass) *
          //                                     -1 / 2.0));
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
    if (pcollWillhappen) {
      CollidedP[0] = collidingP1;
      CollidedP[1] = collidingP2; // i<j always
    }
    if (wcollWillhappen) {
      CollidedP[1] = collidingP2;
    }

    wcollWillhappen = false;
    pcollWillhappen = false;
    // printf("      %d,%d        \n ", collidingP1, collidingP2);
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

    if (noP > 1)
      addP2(&phaseofp2, (Vector2){arr[1].pos.x - arr[0].pos.x,
                                  arr[1].mass * (arr[1].v.x - arr[0].v.x)});
    // addP2(&phaseofp2, (Vector2){arr[1].pos.x,
    // HEIGHT / 2 - 0.1 * arr[1].mass * arr[1].v.x});

    // printf("U:%f,ke:%f ,TE:%f", grphBalance * U, grphBalance * ke,
    // grphBalance * (U + ke));
    // printf("U:%f,ke:%f ,TE:%f\n", U, ke, (U + ke));
    printf("dt:%fTE:%f E(0)-E/ E(0):%f\n)", dt, U + ke,
           ((TE - (U + ke)) / (fabsf(TE))));
    addEnergies(&g, U, ke, U + ke);
    if (panel == 0) {
      float max = fabsf(U) > fabsf(ke) ? U : ke;
      max = fabsf(max) > fabsf(U + ke) ? max : (U + ke);

      // looks better but shifts graph every frame
      grphBalance = (HEIGHT / 2.0 - 50.0f) / (max != 0 ? fabsf(max) : 1.0f);
      drawGraph(&g);
    }
    if (panel == 1) {
      grphBalance = 0.1 * (HEIGHT / 2.0 - 50.0f) / (arr[1].mass * arr[1].v.x);
      grphBalance = 10;
      drawP2(&phaseofp2);
    }
    if (panel == 2) {
      if (focusP == -1 && noP != 0) {
        focusP = 0;
      }
      if (focusP != -1) {
        DrawText(TextFormat(
                     "Particle "
                     "No=%d\nCharge:%d\tMass:%f\nPosition:(%f)i+(%f)"
                     "j\nVelocity:%(f)i+(%f)j\nAcceleration:(%f)i+(%f)j\nKE of "
                     "particle:%f",
                     focusP + 1, arr[focusP].charge, arr[focusP].mass,
                     arr[focusP].pos.x, arr[focusP].pos.y, arr[focusP].v.x,
                     arr[focusP].v.y, arr[focusP].acc.x, arr[focusP].acc.y,
                     0.5 * arr[focusP].mass * Vector2LengthSqr(arr[focusP].v)),
                 WIDTH + 10, 10, 30, BEIGE);
        DrawText(TextFormat("PE:%f\nKE:%f\nTE:%f\ndt:%f\nE(0)-E/ E(0):%f\n", U,
                            ke, U + ke, dt, ((TE - (U + ke)) / (fabsf(TE)))),
                 2 * WIDTH - 100, 10, 30, GREEN);
      }
    }
    // while (fabsf(HEIGHT / 2 - grphBalance * max) > HEIGHT / 2 + 100) {
    //   grphBalance *= 0.1;
    // }
    // I DO IT ABOVE NOW
    // U = 0;
    // ke = 0;
    if (toggleEF) {

      drawEFArrow();
    }
    drawCharge(focusP);
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
