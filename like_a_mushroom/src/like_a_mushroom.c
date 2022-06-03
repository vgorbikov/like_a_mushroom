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

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int RENDERING_AREA_HEIGHT = 28;
const int RENDERING_AREA_WIDTH = 47;
const int BLOCK_SIZE = 18;

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

typedef struct Object
{
	SDL_Rect box;
	float x_speed, y_speed, x_boost, y_boost, weight;
	SDL_Texture *texture;
} Obj;

typedef struct MapBlock
{
	SDL_Rect box;
	SDL_Texture *texture;
	SDL_bool is_death;
	struct MapBlock *next;
} MapBlock;

Obj initObject(SDL_Rect obj_box, float weight)
{
	Obj new_object;
	new_object.box = obj_box;
	new_object.weight = weight;
	new_object.x_speed = 0;
	new_object.x_boost = 0;
	new_object.y_speed = 0;
	new_object.y_boost = 0;
	return new_object;
}

MapBlock *initMapBlock(SDL_Rect block_box, SDL_Texture *texture, SDL_bool deth)
{
	MapBlock *new_object = calloc(1, sizeof(MapBlock));
	new_object->box = block_box;
	new_object->texture = texture;
	new_object->is_death = deth;
	new_object->next = NULL;
	return new_object;
}

typedef struct Map
{
	struct MapBlock *head;
	struct MapBlock *current;
}Map;

int addMapBlock(Map *map, MapBlock *block)
{
	if(map->head == NULL)
	{
		map->head = block;
		map->current = block;
	}
	else
	{
		map->current->next = block;
		map->current = block;
	}
	return 0;
}

int nextBlock(Map *map)
{
	if(map->current != NULL) map->current = map->current->next;
	return 0;
}

int headBlock(Map *map)
{
	map->current = map->head;
	return 0;
}

Map *mapLoad(SDL_Renderer *rend)
{
	FILE *map = fopen("maps/binmap.bin", "rb");
	Map *new_map = calloc(1, sizeof(Map));
	SDL_Texture *stone = IMG_LoadTexture(rend, "textures/stone.png");
	if (map == NULL) printf("No open\n");
	else
	{
		short *width = calloc(1, 2);
		short *height = calloc(1, 1);
		short *block_code = calloc(1, 1);

		fread(width, 2, 1, map);
		fread(height, 1, 1, map);
		printf("Загружена карта %dx%d\n", width[0], height[0]);
		for(int i=0;i<*height;i++)
		{
			for(int j=0;j<*width;j++)
			{
				fread(block_code, 1, 1, map);
				switch (*block_code)
				{
				case 0:;
					SDL_Rect block_box;
					block_box.x = j*BLOCK_SIZE;
					block_box.y = i*BLOCK_SIZE;
					block_box.h = BLOCK_SIZE;
					block_box.w = BLOCK_SIZE;
					SDL_Texture *tex = stone;
					MapBlock *block = initMapBlock(block_box, tex, SDL_FALSE);
					addMapBlock(new_map, block);
					break;
				case 255:
					break;
				}
			}
		}
		fclose(map);
	}
	return new_map;
}



int main(int argc, char *argv[]) {

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	SDL_Window *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_Renderer *rend = NULL;
	SDL_Texture *player = NULL;
	SDL_Texture *background = NULL;
	SDL_bool loopShouldStop = SDL_FALSE;


	rend = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	player = IMG_LoadTexture(rend, "textures/player.png");
	background = IMG_LoadTexture(rend, "textures/background.bmp");
	Map *g_map = mapLoad(rend);

	SDL_Rect playerBox;
	playerBox.x = 0;
	playerBox.y = 0;
	playerBox.h = 30;
	playerBox.w = 30;

	int xspeed = 0;
	int yspeed = 0;

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
					if(event.key.keysym.sym == SDLK_RIGHT) xspeed = 1;
					if(event.key.keysym.sym == SDLK_LEFT) xspeed = -1;
					if(event.key.keysym.sym == SDLK_UP) yspeed = 1;
					if(event.key.keysym.sym == SDLK_DOWN) yspeed = -1;
					break;
				case SDL_KEYUP:
					xspeed = 0;
					yspeed = 0;
					break;
			}
		}

		if(xspeed == 1) playerBox.x += 1;
		if(xspeed == -1) playerBox.x -= 1;
		if(yspeed == 1) playerBox.y -= 1;
		if(yspeed == -1) playerBox.y += 1;


		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, background, NULL, NULL);
		while(g_map->current != NULL)
		{
			SDL_RenderCopy(rend, g_map->current->texture, NULL, &g_map->current->box);
			nextBlock(g_map);
		}
		headBlock(g_map);
		SDL_RenderCopy(rend, player, NULL, &playerBox);
		SDL_RenderPresent(rend);
		SDL_Delay(1);
	}

	SDL_DestroyTexture(player);
	SDL_DestroyTexture(background);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}
