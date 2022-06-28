#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "objects.h"
#include "player.h"
#include "enemies.h"
#include "menu.h"
#include "map_funcs.h"


Map *initMap()
{
	Map *new_map = malloc(sizeof(Map));
	new_map->all_obj = initObjList();
	new_map->movable_obj = initObjList();
	new_map->controlled_obj = initObjList();
	new_map->player = NULL;
	return new_map;
}


Map *mapLoad(SDL_Renderer *rend, int world, int room, ConfigParam *conf)
{
	char buf[100];
	sprintf(buf, "maps/map%i_%i.bin", world, room);
	FILE *map_file = fopen(buf, "rb");
	Map *new_map = initMap();
	sprintf(buf, "maps/background%i_%i.bmp", world, room);
	new_map->bg_tex = IMG_LoadTexture(rend, buf);
	new_map->world = world;
	new_map->room = room;
	if (map_file == NULL) printf("No open\n");
	else
	{
		short *width = calloc(1, 2);
		short *height = calloc(1, 1);
		short *block_code = calloc(1, 1);

		fread(width, 2, 1, map_file);
		fread(height, 1, 1, map_file);
		for(int i=0;i<*height;i++)
		{
			for(int j=0;j<*width;j++)
			{
				fread(block_code, 1, 1, map_file);
				char *path;
				int type;
				int go_out = 0;
				switch (*block_code)
				{
				case STONE_CODE:;
					path = STONE_TEXTURE;
					type = TYPE_MONOLITH;
					break;
				case WOOD_CODE:;
					path = WOOD_TEXTURE;
					type = TYPE_MONOLITH;
					break;
				case PLAYER_CODE:;
					Obj *player = initPlayer(rend, j*BLOCK_SIZE, i*BLOCK_SIZE, conf->player_skin);
//					addObjInList(new_map->all_obj, player);
					addObjInList(new_map->movable_obj, player);
					new_map->player = player;
					continue;
				case MARIO_CODE:;
					Obj *mario = initMario(rend, j*BLOCK_SIZE, i*BLOCK_SIZE);
//					addObjInList(new_map->all_obj, mario);
					addObjInList(new_map->movable_obj, mario);
					addObjInList(new_map->controlled_obj, mario);
					continue;
				case 255:
					continue;
				default:
					printf("Unknown block: %i\n", *block_code);
				}
				if(go_out == 1) continue;
				SDL_Rect block_box;
				block_box.x = j*BLOCK_SIZE;
				block_box.y = i*BLOCK_SIZE;
				block_box.h = BLOCK_SIZE;
				block_box.w = BLOCK_SIZE;
				ObjAnim *tex = initObjAnim(rend, path, NULL);
				Obj *block = initObject(block_box, tex, type);
				addObjInList(new_map->all_obj, block);
			}
		}
		fclose(map_file);
		printf("Load map %dx%d\n", width[0], height[0]);
	}
	headObjInList(new_map->movable_obj);
	while(new_map->movable_obj->current != NULL)
	{
		addObjInList(new_map->all_obj, new_map->movable_obj->current->object);
		nextObjInList(new_map->movable_obj);
	}
	headObjInList(new_map->all_obj);
	headObjInList(new_map->movable_obj);
	return new_map;
}


/**
 * Возвращает объект игрока, инициализированный на карте
 */
Obj *getPlayer(ObjList *map)
{
	headObjInList(map);
	while(map->current->object->type != TYPE_PLAYER)
	{
		nextObjInList(map);
	}
	printf("Get player OK\n");
	return map->current->object;
}

/**
 * Возвращает список всех объектов, которые могут предвигаться
 */
ObjList *getMovable(ObjList *map)
{
	ObjList *movable = initObjList();
	int count = 0;
	headObjInList(map);
	while(map->current != NULL)
	{
		if(map->current->object->type != TYPE_MONOLITH)
		{
			addObjInList(movable, map->current->object);
			count += 1;
		}
		nextObjInList(map);
	}
	headObjInList(map);
	printf("GET MOVABLE OK, count: %i\n", count);
	return movable;
}



int mapRender(Map *map, SDL_Renderer *rend)
{
	SDL_RenderCopy(rend, map->bg_tex, NULL, NULL);
	headObjInList(map->all_obj);
	while(map->all_obj->current != NULL)
	{
		Obj *cur_obj = map->all_obj->current->object;
		SDL_RenderCopyEx(rend, cur_obj->animation->texture, cur_obj->animation->tex_box,
				&cur_obj->box, cur_obj->animation->angle, NULL, cur_obj->animation->flip);
		nextObjInList(map->all_obj);

	}
	headObjInList(map->all_obj);
	return 0;
}


/**
 * Двигает карту по оси x на заданную величину
 */
void moveMap(Map *map, int dx)
{
	headObjInList(map->all_obj);
	while(map->all_obj->current != NULL)
	{
		map->all_obj->current->object->box.x += dx;
		nextObjInList(map->all_obj);
	}
}


void trackThePlayer(Map *map, Obj *player)
{
	int dx = player->box.x + player->box.w/2 - SCREEN_WIDTH/2;
	if(dx > 0) moveMap(map, -dx);
}
