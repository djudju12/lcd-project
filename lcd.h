#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define WINDOW_NAME    "LCD Project"
#define FPS            60
#define WINDOW_WIDTH   1080
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

#define MAXBUTTONS 8

typedef struct {
    Rectangle pos;
    bool clicked;
} Pixel;

typedef struct {
    bool window_active;
    bool grid_active;
    Vector2 mouse_pos;
    Camera2D camera;
} Environment;

typedef void (*Callback)(Environment*);
typedef struct {
    Rectangle bounds;
    const char *label;
    Callback callback;
} Button;

void draw_grid(Pixel[]);
void init_grid(Pixel[]);

void draw_buttons(Button[]);
void init_buttons(Button buttons[]);
void check_buttons(Button[], Vector2 clickpos, Environment *);
bool register_button(Button);

void toggle_pixel(Pixel *);

void update(Environment *);
void render(Environment *);
void ajust_zoom(Environment *, float wheel);

int buttonp = 0;
Button buttons[MAXBUTTONS];

Pixel pixels[TOTAL_CELLS];
