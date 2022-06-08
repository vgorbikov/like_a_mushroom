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

#define BUTTON_PRESSED 1
#define BUTTON_UNPRESSED 0

typedef struct Button
{
	SDL_Keycode code;
	int status;
} Button;


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
				if(event.key.keysym.sym == SDLK_RIGHT) run(player, 500);
				if(event.key.keysym.sym == SDLK_LEFT) run(player, -500);
				if(event.key.keysym.sym == SDLK_UP) player->y_speed = -500;
				if(event.key.keysym.sym == SDLK_DOWN) player->y_speed = 500;
				setObjAnimation(player, RUN_STATUS);
				break;
			case SDL_KEYUP:
				setObjAnimation(player, STATIC_STATUS);
				player->x_speed = 0;
				player->y_speed = 0;
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
	ObjList *g_map = mapLoad(rend);


	//SDL_Texture *pTex = IMG_LoadTexture(rend, "textures/player_anim.png");
	SDL_Rect pTexBox;
	pTexBox.x = 0;
	pTexBox.y = 0;
	pTexBox.w = 100;
	pTexBox.h = 100;
	ObjAnim *playerTex = initObjAnim(rend, "textures/player_anim.png", &pTexBox);
	SDL_Rect playerBox;
	playerBox.x = 20;
	playerBox.y = 16;
	playerBox.h = 50;
	playerBox.w = 50;

	Obj *player = initObject(playerBox, playerTex, 1, SDL_FALSE);
//	addObjInMap(g_map, player);
	addObjInList(g_map, player);


	while (!controlHandler(player))
	{

		movingCalculator(player);
		moveObj(player);

		headObjInList(g_map);
		while(g_map->current != NULL)
		{
			Obj *cur_obj = g_map->current->object;
			if((SDL_HasIntersection(&player->box, &cur_obj->box)&(cur_obj != player)) == SDL_TRUE)
			{
				int dy;
//				if(player->box.x < cur_obj->box.x)
//				{
//					dx = player->box.w - (cur_obj->box.x - player->box.x);
//				}
//				else
//				{
//					dx = cur_obj->box.w - (player->box.x - cur_obj->box.x);
//				}
				if(player->box.y < cur_obj->box.y)
				{
					dy = (player->box.h - (cur_obj->box.y - player->box.y))*(-1);
				}
				else
				{
					dy = cur_obj->box.h - (player->box.y - cur_obj->box.y);
				}
				//player->box.x += dx;
				player->box.y += dy;
			}
			nextObjInList(g_map);

		}

		//if(objectsNearby(player) != 0) printf("Nearby: %i\n", objectsNearby(player));

//		ObjList *nearby = objectsNearby(player);
//		headObjInList(nearby);
//		while(nearby->current != NULL)
//		{
//			SDL_bool intersec = SDL_HasIntersection(&player->box, &nearby->current->object->box);
//			if(intersec != SDL_FALSE)
//			{
//				int dx, dy;
//				if(player->box.x < nearby->current->object->box.x)
//				{
//					dx = player->box.w - (nearby->current->object->box.x - player->box.x);
//				}
//				else
//				{
//					dx = nearby->current->object->box.w - (player->box.x - nearby->current->object->box.x);
//				}
//				if(player->box.y < nearby->current->object->box.y)
//				{
//					dy = player->box.h - (nearby->current->object->box.y - player->box.y);
//				}
//				else
//				{
//					dy = nearby->current->object->box.h - (player->box.y - nearby->current->object->box.y);
//				}
//				printf("Moving...\n");
//				moveObjOnMap(g_map, player, dx, dy);
//				printf("Moving Done\n");
//			}
//		}
//		free(nearby);
		if(player->animation->status == RUN_STATUS) updateRunAnim(player);
		if(player->animation->status == STATIC_STATUS) updateStaticAnim(player);

		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, background, NULL, NULL);
		//printf("Start render\n");
		headObjInList(g_map);
		while(g_map->current != NULL)
		{
			Obj *cur_obj = g_map->current->object;
			SDL_RenderCopyEx(rend, cur_obj->animation->texture, cur_obj->animation->tex_box,
					&cur_obj->box, cur_obj->animation->angle, NULL, cur_obj->animation->flip);
			nextObjInList(g_map);

		}
//		ObjList *writed = initObjList();
//		for(int i=0;i<g_map->height;i++)
//		{
//			for(int j=0;j<g_map->width;j++)
//			{
//				headObjInList(&g_map->tiles[i*g_map->width + j]);
//				while(g_map->tiles[i*g_map->width + j].current != NULL)
//				{
//					if(objInList(writed, g_map->tiles[i*g_map->width + j].current->object) == 0)
//					{
//						OLE *current_OLE = g_map->tiles[i*g_map->width + j].current;
//						SDL_RenderCopyEx(rend, current_OLE->object->animation->texture, current_OLE->object->animation->tex_box,
//								&current_OLE->object->box, current_OLE->object->animation->angle, NULL, current_OLE->object->animation->flip);
//						addObjInList(writed, g_map->tiles[i*g_map->width + j].current->object);
//					}
//					nextObjInList(&g_map->tiles[i*g_map->width + j]);
//				}
//				headObjInList(&g_map->tiles[i*g_map->width + j]);
//			}
//		}
//		delObjList(writed);
		SDL_RenderPresent(rend);
		SDL_Delay(1);
	}
	SDL_DestroyTexture(background);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
