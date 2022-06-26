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


SDL_bool controlHandler(Obj *player)
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
//					player->box.y -= 50;
				}
				break;
			case SDL_KEYUP:
				if(event.key.keysym.sym == SDLK_RIGHT) delEventFromList(player->events, RUN_RIGHT);
				if(event.key.keysym.sym == SDLK_LEFT) delEventFromList(player->events, RUN_LEFT);
				break;
		}
	}
	return SDL_FALSE;
}


int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	/*
	 * Инициализируем окно программы и объект рендера
	 */
	SDL_Window *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_Renderer *rend = NULL;
	SDL_Texture *background = NULL;
	rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	background = IMG_LoadTexture(rend, "textures/background.bmp");
	StatusBar *bar = initStatusBar(1, 1, 300, rend);

	/*
	 * Загружаем карту
	 */
	Map *g_map = mapLoad(rend);


	globalEvent(g_map->movable_obj, GRAVITATION);
//	long int last_frame = 0;

	long int start_game_moment = clock();
	while (!controlHandler(g_map->player))
	{
		nearbyCalculator(g_map->movable_obj);
		enemiesControl(g_map->controlled_obj);

		eventHandler(g_map);

		movingCalculator(g_map->movable_obj);

//		printf("start handle touches\n");
		touchingHandler(g_map);
//		printf("complete handle touches\n");

		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, background, NULL, NULL);


//		float dt = clock() - last_frame;
//		printf("Частота кадров: %f кадров/сек\n", 1000/dt);
//		last_frame = clock();

		trackThePlayer(g_map, g_map->player);
		movingClear(g_map);
		/*
		 * выставляем для всех объектов анимации, соответствующие их состоянию
		 */
		animationHandler(g_map);


//		printf("Start render\n");
		/*
		 * Отправляем все объекты на карте на отрисовку
		 */
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
	SDL_DestroyTexture(background);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
