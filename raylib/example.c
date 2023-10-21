#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define SCREEN_WIDTH   1920
#define SCREEN_HEIGHT  1080
#define SCREEN_YOFFSET 20

#define LCD_OUTER_ROWS 2
#define LCD_OUTER_COLS 16
#define LCD_INNER_ROWS 8
#define LCD_INNER_COLS 5
#define LCD_STARTY     20
#define LCD_STARTX     0
#define TOTAL_CELLS    (LCD_OUTER_ROWS*LCD_OUTER_COLS*LCD_INNER_ROWS*LCD_INNER_COLS)
#define CELL_SIZE      10
#define CELL_MARGIN    2

typedef struct {
    Rectangle pos;
    bool clicked;
} Pixel;

void draw_grid(Pixel pixels[]);
void init_grid(Pixel pixels[]);
void toggle_pixel(Pixel *pixel);

static Pixel pixels[TOTAL_CELLS];

int main()
{
    init_grid(pixels);
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LCD 2000");

    const char *window_name = "LCD Project";
    
    bool window_active = true;
    Vector2 mouse_pos;
    
    SetTargetFPS(60);
    while (!WindowShouldClose() && window_active)
    {
        BeginDrawing();
        {
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            if (window_active) {
                window_active = !GuiWindowBox((Rectangle){ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT }, window_name);
            }

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                mouse_pos = GetMousePosition();
                for (int i = 0; i < TOTAL_CELLS; i++) {
                    if (CheckCollisionPointRec(mouse_pos, pixels[i].pos)) {
                        toggle_pixel(&pixels[i]);
                    }
                }
            }

            draw_grid(pixels);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void init_grid(Pixel pixels[])
{
    float x, y;
    int index, offsetx, offsety;

    offsetx = CELL_SIZE/4;
    offsety = CELL_SIZE/2;
    index = 0;
    for (size_t i = 0; i < LCD_OUTER_ROWS; i++) {
        for (size_t j = 0; j < LCD_OUTER_COLS; j++) {
            for (size_t k = 0; k < LCD_INNER_ROWS; k++) {
                for (size_t l = 0; l < LCD_INNER_COLS; l++) {
                    x = LCD_STARTX + offsetx*j + ((CELL_SIZE+CELL_MARGIN)*(l + LCD_INNER_COLS*j));
                    y = LCD_STARTY + offsety*i + ((CELL_SIZE+CELL_MARGIN)*(k + LCD_INNER_ROWS*i));
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

void toggle_pixel(Pixel *pixel)
{
    pixel->clicked = !pixel->clicked;
}