#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"


ObjTex *initObjTex(SDL_Renderer *rend, char *path, SDL_Rect *rect)
{
	ObjTex *obj_tex = malloc(sizeof(ObjTex));
	obj_tex->tex_box = rect;
	obj_tex->texture = IMG_LoadTexture(rend, path);
	return obj_tex;
}


Obj *initObject(SDL_Rect obj_box, ObjTex *tex, float weight, SDL_bool monolith)
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
