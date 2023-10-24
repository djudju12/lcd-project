#include "lcd.h"
#include "style_cyber.h"

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LCD 2000");
    GuiLoadStyleCyber();

    Environment env = {0};
    env.window_active = true;
    env.grid_active = true;
    env.make_new_code = true;
    env.camera.zoom = 1.0f;
    
    init_grid(grid);
    init_buttons(buttons);
    SetTargetFPS(FPS);

    while (!WindowShouldClose() && env.window_active)
    {
        update(&env);
        BeginDrawing();
            render(&env);
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

    if (IsKeyPressed(KEY_G)) {
        env->grid_active = !env->grid_active;
    }

    // TODO: check if in grid else buttons
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        check_buttons(buttons, env->mouse_pos, env);
        // TODO: move to another function
        for (size_t i = 0; i < LCD_OUTER_ROWS; i++) {
            for (size_t j = 0; j < LCD_OUTER_COLS; j++) {
                for (size_t k = 0; k < LCD_INNER_ROWS; k++) {
                    for (size_t l = 0; l < LCD_INNER_COLS; l++) {
                        if (CheckCollisionPointRec(env->mouse_pos, grid[i][j][k][l].pos)) {
                            grid[i][j][k][l].clicked = !grid[i][j][k][l].clicked;
                        }
                    }
                }
            }
        }
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

        draw_grid(grid);
        draw_buttons(buttons);
        draw_code(grid, env);
    }
    EndMode2D();
}

void init_grid(Grid grid)
{
    float x, y;
    int startx = WINDOW_WIDTH/2 - GRID_WIDTH;
    int starty = 40;

    for (size_t i = 0; i < LCD_OUTER_ROWS; i++) {
        for (size_t j = 0; j < LCD_OUTER_COLS; j++) {
            for (size_t k = 0; k < LCD_INNER_ROWS; k++) {
                for (size_t l = 0; l < LCD_INNER_COLS; l++) {
                    x = startx + CELL_OFFSETX*j + ((CELL_SIZE+CELL_MARGIN)*(l + LCD_INNER_COLS*j));
                    y = starty + CELL_OFFSETY*i + ((CELL_SIZE+CELL_MARGIN)*(k + LCD_INNER_ROWS*i));
                    grid[i][j][k][l].clicked = false;
                    grid[i][j][k][l].pos = (Rectangle) { x, y, CELL_SIZE, CELL_SIZE };
                }
            }
        }
    }
}

void draw_grid(Grid grid)
{
    for (size_t i = 0; i < LCD_OUTER_ROWS; i++) {
        for (size_t j = 0; j < LCD_OUTER_COLS; j++) {
            for (size_t k = 0; k < LCD_INNER_ROWS; k++) {
                for (size_t l = 0; l < LCD_INNER_COLS; l++) {
                    DrawRectangleRec(grid[i][j][k][l].pos, grid[i][j][k][l].clicked ? GREEN : BLACK);
                }
            }
        }
    }
}

void new_code(Environment *env) {
    env->make_new_code = true;
}

void init_buttons(Button buttons[])
{
    Rectangle explosive_bounds = { WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 50, 50 };
    Button explosive = { explosive_bounds, "Gen. Code", new_code };
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

void draw_code(Grid grid, Environment *env)
{
    Rectangle bounds = {
        0,
        WINDOW_HEIGHT/2,
        800,
        800
    };

    char temp[100];
    if (env->make_new_code) {
        code[0] = '\0';
        env->make_new_code = false;
        strcat(code, TEMPLATE);
        strcat(code, "byte customChars[][8] = {\n");
        for (size_t i = 0; i < LCD_OUTER_ROWS; i++) {
            for (size_t j = 0; j < LCD_OUTER_COLS; j++) {
                strcat(code, "\t{");
                for (size_t k = 0; k < LCD_INNER_ROWS; k++) {
                    strcat(code, "B");
                    for (size_t l = 0; l < LCD_INNER_COLS; l++) {
                        strcat(code, grid[i][j][k][l].clicked ? "1" : "0");
                    }
                    if (k < LCD_INNER_ROWS - 1) {
                        strcat(code, ", ");
                    }
                }
                sprintf(temp, "}, // %zux%zu\n", i, j);
                strcat(code, temp);
            }
        }
        strcat(code, "};\n");
    }


    // GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_TOP);
  
        GuiTextBox(bounds, code, MAXCODE, false);
    
    GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT_VERTICAL, TEXT_ALIGN_CENTER);
}