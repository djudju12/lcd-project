run:
	gcc -o lcd_project lcd_project.c `sdl2-config --libs --cflags` -std=c99 -Wall -lm && ./lcd_project

.PHONY: run