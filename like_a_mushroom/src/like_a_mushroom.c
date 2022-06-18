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
#include <time.h>
#include "objects.h"
#include "map_funcs.h"
#include "player.h"


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


SDL_Window *initWindow(int WIDTH, int HEIGHT)
{
	SDL_Window *window = NULL;
	if(SDL_Init(SDL_INIT_VIDEO) < 0) printf("Ошибка инициализации SDL:\n%s\n", SDL_GetError());
	else
	{
		window = SDL_CreateWindow("Moshroom rewenge", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
		if(window == NULL) printf("Ошибка создания окна:\n%s\n", SDL_GetError());
	}
	return window;
}

#define BUTTON_PRESSED 1
#define BUTTON_UNPRESSED 0

int left_arrow, up_arrow, right_arrow, down_arrow;

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
				if(event.key.keysym.sym == SDLK_UP) player->box.y -= 10; //временное решение, пока не реализованы соприкосновения и прыжки
				if(event.key.keysym.sym == SDLK_DOWN) player->box.y += 10;
				break;
			case SDL_KEYUP:
				if(event.key.keysym.sym == SDLK_RIGHT) delEventFromList(player->events, RUN_RIGHT);
				if(event.key.keysym.sym == SDLK_LEFT) delEventFromList(player->events, RUN_LEFT);
//				if(event.key.keysym.sym == SDLK_UP) something;
//				if(event.key.keysym.sym == SDLK_DOWN) something;
				break;
		}
	}
	return SDL_FALSE;
}


int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	SDL_Window *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_Renderer *rend = NULL;
	SDL_Texture *background = NULL;


	rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	background = IMG_LoadTexture(rend, "textures/background.bmp");


	ObjList *g_map = mapLoad(rend); //Загрузка карты


	Obj *player = getPlayer(g_map); //Получение объекта игрока на карте

//	long int last_frame = 0;
	while (!controlHandler(player))
	{
		eventHandler(g_map);

		headObjInList(g_map);
		while(g_map->current != NULL)
		{

			Obj *cur_obj = g_map->current->object;
			if((SDL_HasIntersection(&player->box, &cur_obj->box)&(cur_obj != player)) == SDL_TRUE)
			{
				switch(touchingCalculator(player, cur_obj))
				{
					case TOP_TOUCH:
						printf("top touch\n");
						player->box.y = cur_obj->box.y - player->box.h;
						break;
					case LEFT_TOUCH:
						printf("left touch\n");
						player->box.x = cur_obj->box.x - player->box.w;
						break;
					case RIGHT_TOUCH:
						printf("right touch\n");
						player->box.x = cur_obj->box.x + cur_obj->box.w;;
						break;
					case BOTTOM_TOUCH:
						printf("bottom touch\n");
						player->box.y = cur_obj->box.y + cur_obj->box.h;
						break;
				}
			}
			nextObjInList(g_map);
		}


		animationHandler(g_map);

		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, background, NULL, NULL);

//		float dt = clock() - last_frame;
//		printf("Частота кадров: %f кадров/сек\n", 1000/dt);
//		last_frame = clock();

//		printf("Start render\n");
		mapRender(g_map, rend);

		SDL_RenderPresent(rend);
		SDL_Delay(1);
	}
	SDL_DestroyTexture(background);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
