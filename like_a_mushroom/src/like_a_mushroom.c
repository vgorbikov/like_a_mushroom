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

typedef struct ObjTextureSet
{
	SDL_Texture *texture;
	SDL_Rect *tex_box;
}OTexSet;

typedef struct Object
{
	SDL_Rect box;
	float x_speed, y_speed, x_boost, y_boost, weight;
	SDL_bool monolith;
	SDL_Texture *texture;
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
	return new_object;
}

typedef struct ObjListElem
{
	Obj *object;
	struct ObjListElem *next;
	struct ObjListElem *prev;
}OLE;

OLE *initOLE(Obj *obj)
{
	OLE *returned = malloc(sizeof(OLE));
	returned->object = obj;
	returned->next = NULL;
	returned->prev = NULL;
	return returned;
}

typedef struct Objlist
{
	OLE *head;
	OLE *current;
}ObjList;


ObjList *initObjList()
{
	ObjList *new_list = malloc(sizeof(ObjList));
	new_list->head = NULL;
	new_list->current = NULL;
	return new_list;
}

int addObjInList(ObjList *list, Obj *obj)
{
	OLE *elem = initOLE(obj);
	if(list->head == NULL)
	{
		list->head = elem;
		list->current = elem;
		return 0;
	}
	if(list->current->next != NULL)
	{
		elem->next = list->current->next;
		list->current->next->prev = elem;
	}
	list->current->next = elem;
	elem->prev = list->current;
	list->current = elem;
	return 0;
}

void nextObjInList(ObjList *list)
{
	if(list->current != NULL) list->current = list->current->next;
}

void headObjInList(ObjList *list)
{
	list->current = list->head;
}

int delObjFromList(ObjList *list)
{
	//printf("Deep deleting\n");
	if(list->current != NULL)
	{
		if(list->current->prev != NULL) list->current->prev->next = list->current->next;
		if(list->current->next != NULL) list->current->next->prev = list->current->prev;
		if(list->current == list->head) list->head = list->current->next;
		OLE *new_cur = list->current->next;
		if(list->current != NULL) free(list->current);
		list->current = new_cur;
	//printf("Deep deleting Done\n");
	}
	return 0;
}

int delObjList(ObjList *list)
{
	headObjInList(list);
	while(list->head != NULL) delObjFromList(list);
	if(list != NULL) free(list);
	return 0;
}

typedef struct Map
{
	int height, width;
	ObjList *tiles;
} Map;

void addObjInMap(Map *map, Obj *obj)
{
	if(obj->box.x < 0) obj->box.x = 0;
	if(obj->box.x > (map->width*BLOCK_SIZE - obj->box.w)) obj->box.x = (map->width*BLOCK_SIZE - obj->box.w);
	if(obj->box.y < 0) obj->box.y = 0;
	if(obj->box.y > (map->height*BLOCK_SIZE - obj->box.h)) obj->box.y = (map->height*BLOCK_SIZE - obj->box.h);
	for(int y = obj->box.y; y <= (obj->box.y + obj->box.h - 1); y += BLOCK_SIZE)
	{
		for(int x = obj->box.x; x <= (obj->box.x + obj->box.w - 1); x += BLOCK_SIZE)
		{
			int fx = x/BLOCK_SIZE;
			int fy = y/BLOCK_SIZE;
			//printf("Start add obj in tile %i\n", map->width*fy + fx);
			addObjInList(&map->tiles[map->width*fy + fx], obj);
		}
	}
	//printf("Added\n");
}

void delObjFromMap(Map *map, Obj *obj)
{
	for(int y = obj->box.y; y <= (obj->box.y + obj->box.h - 1); y += BLOCK_SIZE)
		{
			for(int x = obj->box.x; x <= (obj->box.x + obj->box.w - 1); x += BLOCK_SIZE)
			{
				int fx = x/BLOCK_SIZE;
				int fy = y/BLOCK_SIZE;
				//printf("Start deleting obj from field %i\n", map->width*fy + fx);
				while(map->tiles[map->width*fy + fx].current->object != obj) nextObjInList(&map->tiles[map->width*fy + fx]);
				delObjFromList(&map->tiles[map->width*fy + fx]);
				headObjInList(&map->tiles[map->width*fy + fx]);
			}
		}
	//printf("Deleted\n");
}

int objInList(ObjList *list, Obj *obj)
{
	while(list->current != NULL)
	{
		if(list->current->object == obj)
		{
			headObjInList(list);
			return 1;
		}
		nextObjInList(list);
	}
	headObjInList(list);
	return 0;
}

void moveObj(Map *map, Obj *obj, int dx, int dy)
{
	delObjFromMap(map, obj);
	obj->box.x += dx;
	obj->box.y += dy;
	addObjInMap(map, obj);
}

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
		new_map->tiles = calloc(width[0]*height[0], sizeof(ObjList));
		printf("Map length: %i \n", new_map->width);
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
					addObjInMap(new_map, block);
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


	SDL_Texture *playerTex = IMG_LoadTexture(rend, "textures/player_anim.png");
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
					break;
				case SDL_KEYUP:
					player->x_speed = 0;
					player->y_speed = 0;
					break;
			}
		}

		if(player->x_speed == 1)
		{
			moveObj(g_map, player, 1, 0);

		}
		if(player->x_speed == -1) moveObj(g_map, player, -1, 0);
		if(player->y_speed == 1) moveObj(g_map, player, 0, 1);
		if(player->y_speed == -1) moveObj(g_map, player, 0, -1);

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
						SDL_RenderCopy(rend, g_map->tiles[i*g_map->width + j].current->object->texture, NULL, &g_map->tiles[i*g_map->width + j].current->object->box);
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
