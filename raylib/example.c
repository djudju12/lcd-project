#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define WINDOW_NAME    "LCD Project"
#define FPS            60
#define WINDOW_WIDTH   1200
#define WINDOW_HEIGHT  800

#define LCD_OUTER_ROWS 2
#define LCD_OUTER_COLS 16
#define LCD_INNER_ROWS 8
#define LCD_INNER_COLS 5
#define TOTAL_CELLS    (LCD_OUTER_ROWS*LCD_OUTER_COLS*LCD_INNER_ROWS*LCD_INNER_COLS)
#define CELL_SIZE      10
#define CELL_MARGIN    2
#define CELL_OFFSETX   (CELL_SIZE/4)
#define CELL_OFFSETY   (CELL_SIZE/2)

#define GRID_WIDTH     ((CELL_SIZE + CELL_OFFSETX*(LCD_OUTER_COLS-1) + (CELL_SIZE+CELL_MARGIN)*(LCD_INNER_COLS-1 + LCD_INNER_COLS*(LCD_OUTER_COLS-1)))/2)

typedef struct {
    Rectangle pos;
    bool clicked;
} Pixel;

typedef struct {
    bool window_active;
    bool grid_active;
    Vector2 mouse_pos;
} Environment;

void draw_grid(Pixel pixels[]);
void init_grid(Pixel pixels[], int startx, int starty);
void toggle_pixel(Pixel *pixel);
void update(Environment *env);
void render(Environment *env);

static Pixel pixels[TOTAL_CELLS];

int main()
{
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LCD 2000");

    Environment env = { 
        .window_active = true,
        .grid_active = true,
        .mouse_pos = (Vector2) { 0, 0 }
    };
    
    int startx = WINDOW_WIDTH/2 - GRID_WIDTH;
    int starty = 40;

    init_grid(pixels, startx, starty);

    SetTargetFPS(FPS);
    while (!WindowShouldClose() && env.window_active)
    {
        update(&env);
        BeginDrawing();
        {
            render(&env);
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void update(Environment *env)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        env->mouse_pos = GetMousePosition();
        for (int i = 0; i < TOTAL_CELLS; i++) {
            if (CheckCollisionPointRec(env->mouse_pos, pixels[i].pos)) {
                pixels[i].clicked = !pixels[i].clicked;
            }
        }
    }

    if (IsKeyPressed(KEY_G)) {
        env->grid_active = !env->grid_active;
    }
}

void render(Environment *env)
{
    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 
    if (env->window_active) {
        env->window_active = !GuiWindowBox((Rectangle){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, WINDOW_NAME);
    }

    if (env->grid_active) {
        GuiGrid((Rectangle) { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, NULL, 40, 2, NULL);
    }

    draw_grid(pixels);
}

void init_grid(Pixel pixels[], int startx, int starty)
{
    float x, y;
    int index;

    index = 0;
    for (size_t i = 0; i < LCD_OUTER_ROWS; i++) {
        for (size_t j = 0; j < LCD_OUTER_COLS; j++) {
            for (size_t k = 0; k < LCD_INNER_ROWS; k++) {
                for (size_t l = 0; l < LCD_INNER_COLS; l++) {
                    x = startx + CELL_OFFSETX*j + ((CELL_SIZE+CELL_MARGIN)*(l + LCD_INNER_COLS*j));
                    y = starty + CELL_OFFSETY*i + ((CELL_SIZE+CELL_MARGIN)*(k + LCD_INNER_ROWS*i));
                    pixels[index].clicked = false;
                    pixels[index].pos = (Rectangle) { x, y, CELL_SIZE, CELL_SIZE };
                    index++;
                }
            }
        }
    }
}

void draw_grid(Pixel pixels[])
{
    for (int i = 0; i < TOTAL_CELLS; i++) {
        DrawRectangleRec(pixels[i].pos, pixels[i].clicked ? GREEN : BLACK);
    }
}
