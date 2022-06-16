#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "map_funcs.h"
#include "player.h"

ObjList *mapLoad(SDL_Renderer *rend)
{
	FILE *map = fopen("maps/binmap.bin", "rb");
	ObjList *new_map = calloc(1, sizeof(ObjList));
	if (map == NULL) printf("No open\n");
	else
	{
		short *width = calloc(1, 2);
		short *height = calloc(1, 1);
		short *block_code = calloc(1, 1);

		fread(width, 2, 1, map);
		fread(height, 1, 1, map);
		for(int i=0;i<*height;i++)
		{
			for(int j=0;j<*width;j++)
			{
				fread(block_code, 1, 1, map);
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
					Obj *player = initPlayer(rend, j*BLOCK_SIZE, i*BLOCK_SIZE);
					addObjInList(new_map, player);
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
				addObjInList(new_map, block);
			}
		}
		fclose(map);
		printf("Загружена карта %dx%d\n", width[0], height[0]);
	}
	return new_map;
}


Obj *getPlayer(ObjList *map)
{
	headObjInList(map);
	while(map->current->object->type != TYPE_PLAYER)
	{
		nextObjInList(map);
	}
	printf("Get OK\n");
	return map->current->object;
}


int mapRender(ObjList *map, SDL_Renderer *rend)
{
	headObjInList(map);
	while(map->current != NULL)
	{
		Obj *cur_obj = map->current->object;
		SDL_RenderCopyEx(rend, cur_obj->animation->texture, cur_obj->animation->tex_box,
				&cur_obj->box, cur_obj->animation->angle, NULL, cur_obj->animation->flip);
		nextObjInList(map);

	}
	return 0;
}
