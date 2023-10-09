#include <stdio.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define TEMPLATE "// This code is generated by LCD GENERATOR 2000\n"

#define TITLE "LCD Custom Character Generator"

#define WIDTH 800
#define HEIGTH 800
#define FPS 60

#define COLOR_BG 0x181818FF
#define COLOR_CELL 0xFFFFFFFF

#define MAIN_MAX_ROWS 2
#define MAIN_MAX_COLS 16
#define MAIN_POS_X 0
#define MAIN_POS_Y 0

#define ROWS 8
#define COLS 5

#define CELL_SIZE 20
#define CELL_MARG 3
#define CELL_PAD 20

SDL_Renderer *renderer;
Uint8 running = 1;

typedef bool Grid[ROWS][COLS];

void deactive_all(Grid grid)
{
   for (size_t i = 0; i < ROWS; i++)
      for (size_t j = 0; j < COLS; j++)
         grid[i][j] = false;
}

void toggle(Grid grid, int row, int col)
{
   grid[row][col] = !grid[row][col];
}

typedef struct
{
   int active_rows;
   int active_cols;
   Grid main_grid[2][16];
} MainBox;

MainBox main_box;

void translate()
{
   printf("%s", TEMPLATE);
   printf("byte customChars[][8] = {\n");

   for (size_t k = 0; k < main_box.active_rows; k++)
      for (size_t z = 0; z < main_box.active_cols; z++)
      {
         printf("\t{");
         for (size_t i = 0; i < ROWS; i++)
         {
            printf("B");
            for (size_t j = 0; j < COLS; j++)
            {
               printf("%d", main_box.main_grid[k][z][i][j]);
            }
            if (i < ROWS - 1)
               printf(", ");
         }
         printf("}, //  %zux%zu\n", k, z);
      }

   printf("};\n");
}

void set_color(Uint32 color)
{
   Uint8 r = color >> 8 * 3;
   Uint8 g = color >> 8 * 2;
   Uint8 b = color >> 8 * 1;
   Uint8 a = color >> 8 * 0;
   SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void update()
{
}

void main_rect(SDL_Rect *rect)
{
   rect->w = main_box.active_cols * (CELL_SIZE + CELL_MARG) * COLS;
   rect->h = main_box.active_rows * (CELL_SIZE + CELL_MARG) * ROWS;
   rect->x = MAIN_POS_X;
   rect->y = MAIN_POS_Y;
}

void make_main()
{
   SDL_Rect rect = {0};
   main_rect(&rect);

   set_color(0x00FF00FF);
   SDL_RenderFillRect(renderer, &rect);
}

void make_grid(int x, int y, bool grid[ROWS][COLS])
{
   for (size_t i = 0; i < ROWS; i++)
      for (size_t j = 0; j < COLS; j++)
      {
         SDL_Rect rect = {
             .x = (x + CELL_MARG / 2) + (CELL_SIZE + CELL_MARG) * j,
             .y = (y + CELL_MARG / 2) + (CELL_SIZE + CELL_MARG) * i,
             .h = CELL_SIZE,
             .w = CELL_SIZE};

         if (grid[i][j])
            SDL_RenderFillRect(renderer, &rect);
         else
            SDL_RenderDrawRect(renderer, &rect);
      }
}

void make_grids()
{
   set_color(COLOR_CELL);
   for (size_t i = 0; i < main_box.active_rows; i++)
      for (size_t j = 0; j < main_box.active_cols; j++)
      {
         int x = (CELL_SIZE + CELL_MARG) * 5 * j;
         int y = (CELL_SIZE + CELL_MARG) * 8 * i;
         make_grid(x, y, main_box.main_grid[i][j]);
      }
}

void render()
{
   make_main();
   make_grids();
}

void init()
{
   main_box.active_cols = 4;
   main_box.active_rows = 2;

   for (size_t i = 0; i < MAIN_MAX_ROWS; i++)
      for (size_t j = 0; j < MAIN_MAX_COLS; j++)
         deactive_all(main_box.main_grid[i][j]);
}

bool box_clicked(SDL_Rect box, int mousex, int mousey)
{
   SDL_Point mouse_point = {
       .x = mousex,
       .y = mousey};

   return SDL_PointInRect(&mouse_point, &box);
}

void toggle_cel(int mousex, int mousey)
{
   SDL_Rect box;
   main_rect(&box);

   // click outside of the main box
   if (!box_clicked(box, mousex, mousey))
      return;

   // TODO: somar box.x no mousex?
   int w_cell = box.w / main_box.active_cols;
   int h_cell = box.h / main_box.active_rows;

   int row_main = mousey / h_cell;
   int col_main = mousex / w_cell;

   int row_grid = mousey - (row_main * h_cell);
   row_grid /= (CELL_MARG + CELL_SIZE);

   int col_grid = mousex - (col_main * w_cell);
   col_grid /= (CELL_MARG + CELL_SIZE);

   toggle(main_box.main_grid[row_main][col_main], row_grid, col_grid);
}

int main(void)
{
   if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
      goto ERROR;

   SDL_Window *win = SDL_CreateWindow("LCD Generator", 0, 0, WIDTH, HEIGTH, SDL_WINDOW_RESIZABLE);
   if (!win)
      goto ERROR;

   renderer = SDL_CreateRenderer(win, -1, 0);
   if (!renderer)
      goto ERROR;

   init();

   int mx, my;
   while (running)
   {
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
         switch (event.type)
         {
         case SDL_QUIT:
            running = 0;
            break;

         case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
               running = 0;
               break;

            case SDLK_RETURN:
               translate();
               break;

            default:
               break;
            }
            break;

         case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
               SDL_GetMouseState(&mx, &my);
               toggle_cel(mx, my);
            }

            break;

         default:
            break;
         }
      }

      set_color(COLOR_BG);
      SDL_RenderClear(renderer);

      update();
      render();

      SDL_RenderPresent(renderer);
      SDL_Delay(1000 / FPS);
   }

   goto CLEAN_UP;

ERROR:
   printf("[ ERROR ] %s. Going to clean up\n", SDL_GetError());
   goto CLEAN_UP;

CLEAN_UP:
   printf("[ INFO ] cleaning up...\n");
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(win);
   printf("[ INFO ] end of cleaning up.\n");
   return 0;
}