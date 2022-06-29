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
#if (defined(__MINGW64__) || defined(__MINGW32__))
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif
#include <time.h>
#include "status_bar.h"
#include "objects.h"
#include "menu.h"
#include "player.h"
#include "enemies.h"
#include "map_funcs.h"



#define EXIT_CODE 1
#define PLAY_CODE 2
#define OPTIONS_CODE 3
#define MAIN_MENU_CODE 4

#define WASD_CONTROL 0
#define ARROWS_CONTROL 1

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


int gameControlHandler(Obj *player, int control_type)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				return 1;
				break;
			case SDL_KEYDOWN:
				if(control_type == ARROWS_CONTROL)
				{
					if(event.key.keysym.sym == SDLK_RIGHT) addEventInList(player->events, RUN_RIGHT);
					if(event.key.keysym.sym == SDLK_LEFT) addEventInList(player->events, RUN_LEFT);
					if((event.key.keysym.sym == SDLK_UP)&(player->objects_below->head != NULL)) addEventInList(player->events, JUMP);
				}
				if(control_type == WASD_CONTROL)
				{
					if(event.key.keysym.sym == SDLK_d) addEventInList(player->events, RUN_RIGHT);
					if(event.key.keysym.sym == SDLK_a) addEventInList(player->events, RUN_LEFT);
					if((event.key.keysym.sym == SDLK_w)&(player->objects_below->head != NULL)) addEventInList(player->events, JUMP);
				}
				if(event.key.keysym.sym == SDLK_ESCAPE) return MAIN_MENU_CODE;
				break;
			case SDL_KEYUP:
				if(control_type == ARROWS_CONTROL)
				{
					if(event.key.keysym.sym == SDLK_RIGHT) delEventFromList(player->events, RUN_RIGHT);
					if(event.key.keysym.sym == SDLK_LEFT) delEventFromList(player->events, RUN_LEFT);
				}
				if(control_type == WASD_CONTROL)
				{
					if(event.key.keysym.sym == SDLK_d) delEventFromList(player->events, RUN_RIGHT);
					if(event.key.keysym.sym == SDLK_a) delEventFromList(player->events, RUN_LEFT);
				}
				break;
		}
	}
	return 0;
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
				if(((event.key.keysym.sym == SDLK_UP)||(event.key.keysym.sym == SDLK_w))&(mmenu->current_pos > 0)) mmenu->current_pos -= 1;
				if(((event.key.keysym.sym == SDLK_DOWN)||(event.key.keysym.sym == SDLK_s))&(mmenu->current_pos < 2)) mmenu->current_pos += 1;
				break;
		}
	}
	return 0;
}


int optionsControlHandler(OptionsMenu *omenu, ConfigParam *conf)
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
				if(((event.key.keysym.sym == SDLK_UP)||(event.key.keysym.sym == SDLK_w))&(omenu->current_pos > 0)) omenu->current_pos -= 1;
				if(((event.key.keysym.sym == SDLK_DOWN)||(event.key.keysym.sym == SDLK_s))&(omenu->current_pos < 2)) omenu->current_pos += 1;
				if(omenu->current_pos == 0)
				{
					if(((event.key.keysym.sym == SDLK_RIGHT)||(event.key.keysym.sym == SDLK_d))&(omenu->skin < 2)) omenu->skin += 1;
					if(((event.key.keysym.sym == SDLK_LEFT)||(event.key.keysym.sym == SDLK_a))&(omenu->skin > 0)) omenu->skin -= 1;
				}
				if(omenu->current_pos == 1)
				{
					if(((event.key.keysym.sym == SDLK_RIGHT)||(event.key.keysym.sym == SDLK_d))&(omenu->control < 1)) omenu->control += 1;
					if(((event.key.keysym.sym == SDLK_LEFT)||(event.key.keysym.sym == SDLK_a))&(omenu->control > 0)) omenu->control -= 1;
				}
				if((omenu->current_pos == 2)&(event.key.keysym.sym == SDLK_RETURN))
				{
					conf->player_skin = omenu->skin;
					conf->control_type = omenu->control;
					updateConfigFile(conf);
					return MAIN_MENU_CODE;
				}
				break;
		}
	}
	return 0;
}


int gameLoop(SDL_Renderer *rend, ConfigParam *conf, Map *g_map, StatusBar *bar)
{

	globalEvent(g_map->movable_obj, GRAVITATION);

//	long int last_frame = 0;
	long int start_game_moment = clock();

	setLastMoveTime(g_map->all_obj);
	int code = 0;
	while (1)
	{
		code = gameControlHandler(g_map->player, conf->control_type);
		if(code != 0) return code;
		nearbyCalculator(g_map->movable_obj);

		enemiesControl(g_map->controlled_obj);

		int code = eventHandler(g_map);
		if(code == RELOAD)
		{
			bar->lives -= 1;
			updateBarLivesTex(bar, rend);
			return RELOAD;
		}
		if(code == DELETE)
		{
			bar->score += 100;
			updateBarScoreTex(bar, rend);
			printf("+++\n");
		}


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
	return 1;
}



int optionsMenuLoop(SDL_Renderer *rend, OptionsMenu *omenu, ConfigParam *conf)
{
	int menu_event_code;
	while(1)
	{
		menu_event_code = optionsControlHandler(omenu, conf);
		if(menu_event_code != 0) return menu_event_code;
		SDL_RenderClear(rend);
		addToRenderOptionsMenu(omenu, rend, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_RenderPresent(rend);
		SDL_Delay(1);
	}
	return 0;
}



int mainMenuLoop(SDL_Renderer *rend, MainMenu *mmenu)
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
	ConfigParam *conf = getConfigFromFile();
	OptionsMenu *omenu = initOptionsMenu(rend, conf);


	StatusBar *bar = initStatusBar(1, 1, 300, 3, 0, rend);

	int menu_event_code = MAIN_MENU_CODE;
	while(menu_event_code != EXIT_CODE)
	{
		if(menu_event_code == EXIT_CODE) break;
		if(menu_event_code == MAIN_MENU_CODE) menu_event_code = mainMenuLoop(rend, mmenu);
		if((menu_event_code == PLAY_CODE)||(menu_event_code == RELOAD))
		{
			if(bar->lives == 0)
			{
				bar->coins = 0;
				bar->score = 0;
				bar->lives = 3;
				updateBarCoinsTex(bar, rend);
				updateBarScoreTex(bar, rend);
				updateBarLivesTex(bar, rend);
			}
			Map *g_map = mapLoad(rend, 1, 1, conf);
			menu_event_code = gameLoop(rend, conf, g_map, bar);
			delObjList(g_map->controlled_obj);
			delObjList(g_map->movable_obj);
			delObjListDeep(g_map->all_obj);
			SDL_DestroyTexture(g_map->bg_tex);
		}
		if(menu_event_code == OPTIONS_CODE) menu_event_code = optionsMenuLoop(rend, omenu, conf);
	}


	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
