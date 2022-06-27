/*
 ============================================================================
 Name        : mushroom_revenge.c
 Author      : Vadim G
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <time.h>
#include "objects.h"
#include "map_funcs.h"
#include "player.h"
#include "status_bar.h"
#include "enemies.h"
#include "menu.h"


#define EXIT_CODE 1
#define PLAY_CODE 2
#define OPTIONS_CODE 3
#define MAIN_MENU_CODE 4

SDL_Window *initWindow(int WIDTH, int HEIGHT)
{
	SDL_Window *window = NULL;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) printf("Ошибка инициализации SDL:\n%s\n", SDL_GetError());
	else
	{
		window = SDL_CreateWindow("Moshroom rewenge", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
		if(window == NULL) printf("Ошибка создания окна:\n%s\n", SDL_GetError());
	}
	if(TTF_Init() != 0) printf("TTF_Init ERROR\n");
	return window;
}


SDL_bool gameControlHandler(Obj *player)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return SDL_TRUE;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_RIGHT) addEventInList(player->events, RUN_RIGHT);
				if(event.key.keysym.sym == SDLK_LEFT) addEventInList(player->events, RUN_LEFT);
				if(event.key.keysym.sym == SDLK_UP)
				{
					if(player->objects_below->head != NULL) addEventInList(player->events, JUMP);
				}
				if(event.key.keysym.sym == SDLK_1) addEventInList(player->events, DEATH);
				break;
			case SDL_KEYUP:
				if(event.key.keysym.sym == SDLK_RIGHT) delEventFromList(player->events, RUN_RIGHT);
				if(event.key.keysym.sym == SDLK_LEFT) delEventFromList(player->events, RUN_LEFT);
				break;
		}
	}
	return SDL_FALSE;
}


int menuControlHandler(MainMenu *mmenu)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return EXIT_CODE;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_RETURN)
				{
					if(mmenu->current_pos == 0) return PLAY_CODE;
					if(mmenu->current_pos == 1) return OPTIONS_CODE;
					if(mmenu->current_pos == 2) return EXIT_CODE;
				}
				if((event.key.keysym.sym == SDLK_UP)&(mmenu->current_pos > 0)) mmenu->current_pos -= 1;
				if((event.key.keysym.sym == SDLK_DOWN)&(mmenu->current_pos < 2)) mmenu->current_pos += 1;
				if(event.key.keysym.sym == SDLK_ESCAPE) return PLAY_CODE;
				break;
		}
	}
	return 0;
}


void gameLoop(SDL_Renderer *rend, Map *g_map)
{
	StatusBar *bar = initStatusBar(g_map->world, g_map->room, 300, rend);
	globalEvent(g_map->movable_obj, GRAVITATION);

//	long int last_frame = 0;
	long int start_game_moment = clock();

	setLastMoveTime(g_map->all_obj);
	while (!gameControlHandler(g_map->player))
	{
		nearbyCalculator(g_map->movable_obj);

		enemiesControl(g_map->controlled_obj);

		eventHandler(g_map);

		movingCalculator(g_map->movable_obj);

		touchingHandler(g_map);

		SDL_RenderClear(rend);

//		float dt = clock() - last_frame;
//		printf("Частота кадров: %f кадров/сек\n", 1000/dt);
//		last_frame = clock();

		trackThePlayer(g_map, g_map->player);

		movingClear(g_map);

		animationHandler(g_map);

		mapRender(g_map, rend);

		long int cur_t = MAP1_TIME - (clock() - start_game_moment)/1000;
		if(cur_t != bar->time)
		{
			bar->time = cur_t;
			updateBarTimeTex(bar, rend);
		}
		addSBarToRender(bar, rend, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_RenderPresent(rend);
		SDL_Delay(1);
	}
}


int menuLoop(SDL_Renderer *rend, MainMenu *mmenu)
{
	int menu_event_code;
	while(1)
	{
		menu_event_code = menuControlHandler(mmenu);
		if(menu_event_code != 0) return menu_event_code;
		SDL_RenderClear(rend);
		addToRenderMainMenu(mmenu, rend, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_RenderPresent(rend);
		SDL_Delay(1);
	}
	return 0;
}


int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	/*
	 * Инициализируем окно программы и объект рендера
	 */
	SDL_Window *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_Renderer *rend = NULL;
	rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	MainMenu *mmenu = initMainMenu(rend);

	Map *g_map = mapLoad(rend, 1, 1);

	int menu_event_code;
	while(1)
	{
		menu_event_code = menuLoop(rend, mmenu);
		if(menu_event_code == 1) break;
		if(menu_event_code == 2) gameLoop(rend, g_map);
	}


	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
