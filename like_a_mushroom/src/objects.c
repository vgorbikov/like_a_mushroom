#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "map_funcs.h"
#include "player.h"


ObjAnim *initObjAnim(SDL_Renderer *rend, char *path, SDL_Rect *rect)
{
	ObjAnim *obj_tex = malloc(sizeof(ObjAnim));
	obj_tex->tex_box = rect;
	obj_tex->texture = IMG_LoadTexture(rend, path);
	obj_tex->status = STATIC_STATUS;
	obj_tex->angle = 0;
	obj_tex->flip = SDL_FLIP_NONE;
	obj_tex->start_moment = clock();
	return obj_tex;
}


Obj *initObject(SDL_Rect obj_box, ObjAnim *tex, float weight, int type)
{
	Obj *new_object = malloc(sizeof(Obj));
	new_object->box = obj_box;
	new_object->weight = weight;
	new_object->x_speed = 0;
	new_object->x_boost = 0;
	new_object->y_speed = 0;
	new_object->y_boost = 0;
	new_object->animation = tex;
	new_object->last_xmove = clock();
	new_object->last_ymove = clock();
	new_object->status = STATIC_STATUS;
	new_object->type = type;
	return new_object;
}


OLE *initOLE(Obj *obj)
{
	OLE *returned = malloc(sizeof(OLE));
	returned->object = obj;
	returned->next = NULL;
	returned->prev = NULL;
	return returned;
}


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
		free(list->current);
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


void setObjAnimation(Obj *obj, int animation_type)
{
	if(obj->animation->status != animation_type)
	{
		obj->animation->status = animation_type;
		obj->animation->start_moment = clock();
	}
}


/**
 * Вычисляет перемещение объекта с момента отрисовки последнего кадра
 */
int movingCalculator(Obj *obj)
{
	long int time = clock();
	float tempxdt = (time - obj->last_xmove);
	float xdt = tempxdt/1000;
	if(!((xdt*obj->x_speed < 1) & (xdt*obj->x_speed > -1) & (obj->x_speed != 0)))
	{
		obj->moving.x = obj->x_speed*xdt + obj->x_boost*xdt*xdt/2;
		obj->last_xmove = clock();
	}


	float tempydt = (time - obj->last_ymove);
	float ydt = tempydt/1000;
	if(!((ydt*obj->y_speed < 1) & (ydt*obj->y_speed > -1) & (obj->y_speed != 0)))
	{
		obj->moving.y = obj->y_speed*ydt + obj->y_boost*ydt*ydt/2;
		obj->last_ymove = clock();
	}
	return 0;
}


/**
 * Применяет заданное объекту перемещение
 */
void moveObj(Obj *obj)
{
//	moveObjOnMap(obj->map, obj, obj->moving.x, obj->moving.y);
	obj->box.x += obj->moving.x;
	obj->box.y += obj->moving.y;
	obj->moving.x = 0;
	obj->moving.y = 0;
}


/**
 * Возвращает список объектов поблизости
 * (от объекта, передаваемого в качестве аргумента)
 */
ObjList *objectsNearby(Obj *obj)
{
	Map *map = obj->map;
	ObjList *nearby = initObjList();
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
			headObjInList(&map->tiles[map->width*fy + fx]);
			while(map->tiles[map->width*fy + fx].current != NULL)
			{
				if((map->tiles[map->width*fy + fx].current->object != obj)&(!objInList(nearby, map->tiles[map->width*fy + fx].current->object)))
				{
					addObjInList(nearby, map->tiles[map->width*fy + fx].current->object);
				}
				nextObjInList(&map->tiles[map->width*fy + fx]);
			}
		}
	}
	return nearby;
}


/**
 * Возвращает пересечение двух объектов, если оно есть
 * В противном случае NULL
 */
SDL_Rect *touchingCalculator(Obj *obj1, Obj *obj2)
{
	SDL_Rect *result = malloc(sizeof(SDL_Rect));
	if(SDL_IntersectRect(&obj1->box, &obj2->box, result) == SDL_FALSE) return NULL;
	return result;
}


//void touchingDetector(Map *map)
//{
//	for(int i=0;i<map->height;i++)
//	{
//		for(int j=0;j<map->width;j++)
//		{
//			ObjList *tile = &map->tiles[map->width + j];
//			headObjInList(&map->tiles[i*map->width + j]);
//			while(tile->current != NULL)
//			{
//				Obj *curObj = tile->current->object;
//
//				nextObjInList(tile);
//			}
//		}
//	}
//
//}
