#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "map_funcs.h"
#include "player.h"



void initLine(Line line, int x1, int y1, int x2, int y2)
{
	//Line *line = malloc(sizeof(Line));
	line.begin.x = x1;
	line.begin.y = y1;
	line.end.x = x2;
	line.end.y = y2;
}


Line *getMovingLines(SDL_Rect *rect, SDL_Point *moving)
{
	Line *lines = malloc(4*sizeof(Line));
	//printf("Start initialization lines\n");
	initLine(lines[0], (rect->x - moving->x), (rect->y - moving->y), rect->x, rect->y);
	initLine(lines[1], (rect->x + rect->w - moving->x), (rect->y - moving->y), (rect->x + rect->w), rect->y);
	initLine(lines[2], (rect->x + rect->w - moving->x), (rect->y + rect->h - moving->y), (rect->x + rect->w), (rect->y + rect->h));
	initLine(lines[3], (rect->x - moving->x), (rect->y + rect->h - moving->y), rect->x, (rect->y + rect->h));
	return lines;
}


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
 * Придаёт объекту перемещение
 */
int movingCalculator(Obj *obj)
{
	long int time = clock();
	float tempxdt = (time - obj->last_xmove);
	float xdt = tempxdt/1000;
	if(!(((obj->x_speed*xdt + obj->x_boost*xdt*xdt/2) < 1) & ((obj->x_speed*xdt + obj->x_boost*xdt*xdt/2) > -1) & (obj->x_speed != 0)))
	{
		obj->moving.x = obj->x_speed*xdt + obj->x_boost*xdt*xdt/2;
		obj->last_xmove = clock();
	}


	float tempydt = (time - obj->last_ymove);
	float ydt = tempydt/1000;
	if(!(((obj->y_speed*ydt + obj->y_boost*ydt*ydt/2) < 1) & ((obj->y_speed*ydt + obj->y_boost*ydt*ydt/2) > -1) & (obj->y_speed != 0)))
	{
		obj->moving.y = obj->y_speed*ydt + obj->y_boost*ydt*ydt/2;
		obj->last_ymove = clock();
	}
//	obj->box.x += obj->moving.x;
//	obj->box.y += obj->moving.y;
	return 0;
}


void moveObj(Obj *obj)
{
	obj->box.x += obj->moving.x;
	obj->box.y += obj->moving.y;
	obj->moving.x = 0;
	obj->moving.y = 0;
}


///**
// * Возвращает тип касания двух объектов, если оно есть
// * В противном случае 0
// * Первый аргумент - объект совершающий касание
// * Второй аргумент - объект "принимающий" касание
// */
int touchingCalculator(Obj *donor, Obj *acceptor)
{
	donor->box.x += donor->moving.x;
	donor->box.y += donor->moving.y;
	int movx = donor->moving.x;
	int movy = donor->moving.y;
	SDL_Rect lacmus;
	if((SDL_HasIntersection(&donor->box, &acceptor->box)&(donor != acceptor)) == SDL_TRUE)
	{
		float fx = movx;
		float fy = movy;
		float k = fy/fx;
		float kx = fx/fy;
		float dy = 0;
		float dx = 0;
		while(movx != 0)
		{
			if(movx > 0)
			{
				dy -= k;
				donor->box.x -= 1;
				movx -=1;
				if(abs(dy/1) > 0)
				{
					dy -= (abs(dy) - 1)*(abs(dy)/dy);
					donor->box.y += abs(dy)/dy;
					movy += abs(dy)/dy;
				}
			}
			if(movx < 0)
			{
				dy += k;
				donor->box.x += 1;
				movx +=1;
				if(abs(dy/1) > 0)
				{
					dy -= (abs(dy) - 1)*(abs(dy)/dy);
					donor->box.y += abs(dy)/dy;
					movy += abs(dy)/dy;
				}
			}
			SDL_IntersectRect(&donor->box, &acceptor->box, &lacmus);
			if((lacmus.h == 1)||(lacmus.w == 1)) break;
		}
		while(movy != 0)
		{
			if(movy > 0)
			{
				dx -= kx;
				donor->box.y -= 1;
				movy -=1;
				if(abs(dx/1) > 0)
				{
					dx -= (abs(dx) - 1)*(abs(dx)/dx);
					donor->box.x += abs(dx)/dx;
					movx += abs(dx)/dx;
				}
			}
			if(movy < 0)
			{
				dx += kx;
				donor->box.y += 1;
				movy +=1;
				if(abs(dx/1) > 0)
				{
					dx -= (abs(dx) - 1)*(abs(dx)/dx);
					donor->box.x += abs(dx)/dx;
					movx += abs(dx)/dx;
				}
			}
			SDL_IntersectRect(&donor->box, &acceptor->box, &lacmus);
			if((lacmus.h == 1)||(lacmus.w == 1)) break;
			}
	}

	if(lacmus.h == 1)
	{
		if(lacmus.y == acceptor->box.y) return TOP_TOUCH;
		if(lacmus.y == (acceptor->box.y + acceptor->box.h - 1)) return BOTTOM_TOUCH;
	}
	if(lacmus.w == 1)
	{
		if(lacmus.x == acceptor->box.x) return LEFT_TOUCH;
		if(lacmus.x == (acceptor->box.x + acceptor->box.w - 1)) return RIGHT_TOUCH;
	}
	return 0;
}

