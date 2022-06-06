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
#include "objects.h"
#include "map_funcs.h"
#include "player.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


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


int main(int argc, char *argv[]) {

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	SDL_Window *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_Renderer *rend = NULL;
	SDL_Texture *background = NULL;
	SDL_bool loopShouldStop = SDL_FALSE;


	rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	background = IMG_LoadTexture(rend, "textures/background.bmp");
	Map *g_map = mapLoad(rend);


	//SDL_Texture *pTex = IMG_LoadTexture(rend, "textures/player_anim.png");
	SDL_Rect pTexBox;
	pTexBox.x = 0;
	pTexBox.y = 0;
	pTexBox.w = 100;
	pTexBox.h = 100;
	ObjAnim *playerTex = initObjAnim(rend, "textures/player_anim.png", &pTexBox);
	SDL_Rect playerBox;
	playerBox.x = 0;
	playerBox.y = 0;
	playerBox.h = 50;
	playerBox.w = 50;

	Obj *player = initObject(playerBox, playerTex, 1, SDL_FALSE);
	addObjInMap(g_map, player);


	while (!loopShouldStop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					loopShouldStop = SDL_TRUE;
					break;
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_RIGHT) player->x_speed = 1;
					if(event.key.keysym.sym == SDLK_LEFT) player->x_speed = -1;
					if(event.key.keysym.sym == SDLK_UP) player->y_speed = -1;
					if(event.key.keysym.sym == SDLK_DOWN) player->y_speed = 1;
					setObjAnimation(player, RUN_STATUS);
					break;
				case SDL_KEYUP:
					setObjAnimation(player, STATIC_STATUS);
					player->x_speed = 0;
					player->y_speed = 0;
					break;
			}
		}

		if(player->x_speed == 1)
		{
			moveObj(player, 1, 0);

		}
		if(player->x_speed == -1) moveObj(player, -1, 0);
		if(player->y_speed == 1) moveObj(player, 0, 1);
		if(player->y_speed == -1) moveObj(player, 0, -1);

		if(player->animation->status == RUN_STATUS) updateRunAnim(player);
		if(player->animation->status == STATIC_STATUS) updateStaticAnim(player);

		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, background, NULL, NULL);
		//printf("Start render\n");
		ObjList *writed = initObjList();
		for(int i=0;i<g_map->height;i++)
		{
			for(int j=0;j<g_map->width;j++)
			{
				headObjInList(&g_map->tiles[i*g_map->width + j]);
				while(g_map->tiles[i*g_map->width + j].current != NULL)
				{
					if(objInList(writed, g_map->tiles[i*g_map->width + j].current->object) == 0)
					{
						SDL_RenderCopy(rend, g_map->tiles[i*g_map->width + j].current->object->animation->texture, g_map->tiles[i*g_map->width + j].current->object->animation->tex_box, &g_map->tiles[i*g_map->width + j].current->object->box);
						addObjInList(writed, g_map->tiles[i*g_map->width + j].current->object);
					}
					nextObjInList(&g_map->tiles[i*g_map->width + j]);
				}
				headObjInList(&g_map->tiles[i*g_map->width + j]);
			}
		}
		delObjList(writed);
		SDL_RenderPresent(rend);
		SDL_Delay(1);
	}
	SDL_DestroyTexture(background);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
