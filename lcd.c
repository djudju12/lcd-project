#include "lcd.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LCD 2000");

    Environment env = {0};
    env.window_active = true;
    env.grid_active = true;
    env.camera.zoom = 1.0f;
    
    
    init_grid(pixels);
    init_buttons(buttons);
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
    float wheel;
    env->mouse_pos = GetScreenToWorld2D(GetMousePosition(), env->camera);

    if ((wheel = GetMouseWheelMove()) != 0) {
        ajust_zoom(env, wheel);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        check_buttons(buttons, env->mouse_pos, env);
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
    BeginMode2D(env->camera);
    {
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 
        if (env->window_active) {
            env->window_active = !GuiWindowBox((Rectangle){ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, WINDOW_NAME);
        }

        if (env->grid_active) {
            GuiGrid((Rectangle) { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, NULL, 40, 2, NULL);
        }

        draw_grid(pixels);
        draw_buttons(buttons);
    }
    EndMode2D();
}

void init_grid(Pixel pixels[])
{
    float x, y;
    int index;
    int startx = WINDOW_WIDTH/2 - GRID_WIDTH;
    int starty = 40;

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

void explode(Environment *env) {
    printf("EXPLODE\n");
    env->window_active = false;
}

void init_buttons(Button buttons[])
{
    Rectangle explosive_bounds = { WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 50, 50 };
    Button explosive = { explosive_bounds, "EXPLODE", explode };
    register_button(explosive);
}

bool register_button(Button button) {
    if (buttonp < MAXBUTTONS) {
        buttons[buttonp] = button;
        buttonp++;
        return true;
    } else {
        return false;
    }
}

void draw_buttons(Button buttons[]) 
{
    for (size_t i = 0; i < buttonp; i++) {
        GuiButton(buttons[i].bounds, buttons[i].label);
    }
}

void check_buttons(Button buttons[], Vector2 clickpos, Environment *env)
{ 
    for (size_t i = 0; i < buttonp; i++) {
        if (CheckCollisionPointRec(clickpos, buttons[i].bounds)) {
            buttons[i].callback(env);
            return;
        }
    }
}

void ajust_zoom(Environment *env, float wheel)
{
    const float zoom_increment = 0.125f;
    env->camera.offset = GetMousePosition();
    env->camera.target = env->mouse_pos;
    env->camera.zoom += (wheel*zoom_increment);

    if (env->camera.zoom < 1.0f) env->camera.zoom = 1.0f;
}