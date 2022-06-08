#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "map_funcs.h"


void addObjInMap(Map *map, Obj *obj)
{
	obj->map = map;
	if(obj->box.x < 0) obj->box.x = 0;
	if(obj->box.x > (map->width*BLOCK_SIZE - obj->box.w)) obj->box.x = (map->width*BLOCK_SIZE - obj->box.w);
	if(obj->box.y < 0) obj->box.y = 0;
	if(obj->box.y > (map->height*BLOCK_SIZE - obj->box.h)) obj->box.y = (map->height*BLOCK_SIZE - obj->box.h);
	int dx = obj->box.w/(obj->box.w/BLOCK_SIZE);
	int dy = obj->box.h/(obj->box.h/BLOCK_SIZE);
	for(int y = obj->box.y; y <= (obj->box.y + obj->box.h); y += dy)
	{
		if(y == (obj->box.y + obj->box.h)) y = (obj->box.y + obj->box.h - 1);
		for(int x = obj->box.x; x <= (obj->box.x + obj->box.w); x += dx)
		{
			if(x == (obj->box.x + obj->box.w)) x = (obj->box.x + obj->box.w - 1);
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
	obj->map = NULL;
	int dx = obj->box.w/(obj->box.w/BLOCK_SIZE);
	int dy = obj->box.h/(obj->box.h/BLOCK_SIZE);
	for(int y = obj->box.y; y <= (obj->box.y + obj->box.h); y += dy)
	{
		if(y == (obj->box.y + obj->box.h)) y = (obj->box.y + obj->box.h - 1);
		for(int x = obj->box.x; x <= (obj->box.x + obj->box.w); x += dx)
		{
			if(x == (obj->box.x + obj->box.w)) x = (obj->box.x + obj->box.w - 1);
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


void moveObjOnMap(Map *map, Obj *obj, int dx, int dy)
{
	delObjFromMap(map, obj);
	obj->box.x += dx;
	obj->box.y += dy;
	addObjInMap(map, obj);
}


ObjList *mapLoad(SDL_Renderer *rend)
{
	FILE *map = fopen("maps/binmap.bin", "rb");
	//SDL_Texture *stone = IMG_LoadTexture(rend, "textures/stone.png");
	ObjList *new_map = calloc(1, sizeof(ObjList));
	if (map == NULL) printf("No open\n");
	else
	{
		short *width = calloc(1, 2);
		short *height = calloc(1, 1);
		short *block_code = calloc(1, 1);

		fread(width, 2, 1, map);
		fread(height, 1, 1, map);
//		new_map->height = *height;
//		new_map->width = *width;
//		new_map->tiles = calloc(width[0]*height[0], sizeof(ObjList));
//		printf("Map length: %i \n", new_map->width);
		for(int i=0;i<*height;i++)
		{
			for(int j=0;j<*width;j++)
			{
				fread(block_code, 1, 1, map);
				char *path;
				switch (*block_code)
				{
				case STONE_CODE:;
					path = STONE_TEXTURE;
					break;
				case WOOD_CODE:;
					path = WOOD_TEXTURE;
					break;
				case 255:
					continue;
					break;
				default:
					printf("Unknown block: %i\n", *block_code);
				}
				SDL_Rect block_box;
				block_box.x = j*BLOCK_SIZE;
				block_box.y = i*BLOCK_SIZE;
				block_box.h = BLOCK_SIZE;
				block_box.w = BLOCK_SIZE;
				ObjAnim *tex = initObjAnim(rend, path, NULL);
				Obj *block = initObject(block_box, tex, 1, TYPE_MONOLITH);
				//printf("block %i\n", i*new_map->width+j);
				addObjInList(new_map, block);
			}
		}
		fclose(map);
		printf("Загружена карта %dx%d\n", width[0], height[0]);
	}
	return new_map;
}