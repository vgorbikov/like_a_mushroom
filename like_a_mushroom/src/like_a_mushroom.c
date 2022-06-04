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
	SDL_bool monolith;
	SDL_Texture *texture;
	struct Object *prev_object_nearby;
	struct Object *next_object_nearby; //указатель на объект, находящийся с данным на одном поле
} Obj;


Obj *initObject(SDL_Rect obj_box, SDL_Texture *tex, float weight, SDL_bool monolith)
{
	Obj *new_object = malloc(sizeof(Obj));
	new_object->box = obj_box;
	new_object->weight = weight;
	new_object->x_speed = 0;
	new_object->x_boost = 0;
	new_object->y_speed = 0;
	new_object->y_boost = 0;
	new_object->texture = tex;
	new_object->prev_object_nearby = NULL;
	new_object->next_object_nearby = NULL;
	return new_object;
}

typedef struct MapField
{
	Obj *head;
	Obj *current;
}MapField;


MapField *initMapField()
{
	MapField *new_map = malloc(sizeof(MapField));
	new_map->head = NULL;
	new_map->current = NULL;
	return new_map;
}

int addObj(MapField *map, Obj *obj)
{
	if(map->head == NULL)
	{
		map->head = obj;
		map->current = obj;
		return 0;
	}
	if(map->current->next_object_nearby != NULL) obj->next_object_nearby = map->current->next_object_nearby;
	map->current->next_object_nearby = obj;
	obj->prev_object_nearby = map->current;
	map->current = obj;
	return 0;
}

void nextObj(MapField *map)
{
	if(map->current != NULL) map->current = map->current->next_object_nearby;
}

void headObj(MapField *map)
{
	map->current = map->head;
}

typedef struct Map
{
	int height, width;
	MapField *fields;
} Map;

Map *mapLoad(SDL_Renderer *rend)
{
	FILE *map = fopen("maps/binmap.bin", "rb");
	SDL_Texture *stone = IMG_LoadTexture(rend, "textures/stone.png");
	Map *new_map = malloc(sizeof(Map));
	if (map == NULL) printf("No open\n");
	else
	{
		short *width = calloc(1, 2);
		short *height = calloc(1, 1);
		short *block_code = calloc(1, 1);

		fread(width, 2, 1, map);
		fread(height, 1, 1, map);
		new_map->height = *height;
		new_map->width = *width;
		new_map->fields = calloc(width[0]*height[0], sizeof(MapField));
		printf("Start_load: %i \n", new_map->width);
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
					Obj *block = initObject(block_box, tex, 1, SDL_TRUE);
					//printf("block %i\n", i*new_map->width+j);
					addObj(&new_map->fields[i*new_map->width+j], block);
					break;
				case 255:
					break;
				}
			}
		}
		fclose(map);
		printf("Загружена карта %dx%d\n", width[0], height[0]);
	}
	return new_map;
}

void physCalc()
{

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
	player = IMG_LoadTexture(rend, "textures/player_anim.png");
	background = IMG_LoadTexture(rend, "textures/background.bmp");
	Map *g_map = mapLoad(rend);

	SDL_Rect playerBox;
	playerBox.x = 0;
	playerBox.y = 0;
	playerBox.h = 50;
	playerBox.w = 50;

	int xspeed = 0;
	int yspeed = 0;
	int steps = 0;
	int frame = 0;

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
					steps = 0;
					frame = 0;
					break;
			}
		}

		if(xspeed == 1)
		{
			playerBox.x += 1;
			steps +=1;
			if(steps > BLOCK_SIZE*8) steps = 0;
			frame = steps/(BLOCK_SIZE*8/6) + 1;
		}
		if(xspeed == -1) playerBox.x -= 1;
		if(yspeed == 1) playerBox.y -= 1;
		if(yspeed == -1) playerBox.y += 1;

		SDL_Rect FrameBox;
		FrameBox.x = 100*frame;
		FrameBox.y = 0;
		FrameBox.h = 100;
		FrameBox.w = 100;

		SDL_RenderClear(rend);
		SDL_RenderCopy(rend, background, NULL, NULL);
		//printf("Start render\n");
		for(int i=0;i<g_map->height;i++)
		{
			for(int j=0;j<g_map->width;j++)
			{
				//printf("Block %i \n", i*g_map->width + j);
				MapField f = g_map->fields[i*g_map->width + j];
				if(f.head != NULL) SDL_RenderCopy(rend, f.current->texture, NULL, &f.current->box);
			}
		}
		SDL_RenderCopy(rend, player, &FrameBox, &playerBox);
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
