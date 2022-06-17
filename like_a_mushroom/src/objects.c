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


ObjEvent *initEvent(int event_code)
{
	ObjEvent *event = calloc(1, sizeof(ObjEvent));
	event->start_moment = clock();
	event->event_code = event_code;
	return event;
}


Obj *initObject(SDL_Rect obj_box, ObjAnim *tex, int type)
{
	Obj *new_object = malloc(sizeof(Obj));
	new_object->box = obj_box;
	new_object->x_speed = 0;
	new_object->y_speed = 0;
	new_object->animation = tex;
	new_object->last_xmove = clock();
	new_object->last_ymove = clock();
	new_object->events = initEventList();
	new_object->type = type;
	return new_object;
}


/**
 * Возвращает элемент-обёртку списка.
 * Внутренняя функция.
 */
OLE *initOLE(Obj *obj)
{
	OLE *returned = malloc(sizeof(OLE));
	returned->object = obj;
	returned->next = NULL;
	returned->prev = NULL;
	return returned;
}


/**
 * Возвращает пустой список объектов
 */
ObjList *initObjList()
{
	ObjList *new_list = malloc(sizeof(ObjList));
	new_list->head = NULL;
	new_list->current = NULL;
	return new_list;
}


/**
 * Добавляет объект в указанный список
 * (На текущую позицию)
 */
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


/**
 * Перемещает указатель на текущий объект на позицию вперёд
 */
void nextObjInList(ObjList *list)
{
	if(list->current != NULL) list->current = list->current->next;
}


/**
 * Перемещает указатель на текущий объект в начало списка
 */
void headObjInList(ObjList *list)
{
	list->current = list->head;
}


/**
 * Удаляет текущий объект из списка
 */
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


/**
 * Удаляет список объектов
 */
int delObjList(ObjList *list)
{
	headObjInList(list);
	while(list->head != NULL) delObjFromList(list);
	if(list != NULL) free(list);
	return 0;
}


/**
 * Возвращает 1, если указанный объект есть в указанном списке.
 * В противном случае 0.
 */
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


/**
 * Функция для создания элемента-оболочки в списке событий.
 * Внутренняя функция. В хорошем случае вы не увидите её почти нигде)
 */
ELE *initELE(ObjEvent *event)
{
	ELE *returned = malloc(sizeof(ELE));
	returned->event = event;
	returned->next = NULL;
	returned->prev = NULL;
	return returned;
}


/**
 * Функция для инициализации списка событий.
 * Возвращает пустой список событий.
 */
EventList *initEventList()
{
	EventList *new_list = malloc(sizeof(EventList));
	new_list->head = NULL;
	new_list->current = NULL;
	return new_list;
}



/**
 * Переводит указатель current на одну позицию вперёд в списке событий
 */
void nextEventInList(EventList *list)
{
	if(list->current != NULL) list->current = list->current->next;
}


/**
 * Переводит указатель current на головной элемент в списке событий
 */
void headEventInList(EventList *list)
{
	list->current = list->head;
}


/**
 * Возвращает 1, если событие с указанным кодом есть в указанном списке.
 * В противном случае 0.
 */
int eventInList(EventList *list, int event_code)
{
	while(list->current != NULL)
	{
		if(list->current->event->event_code == event_code)
		{
			headEventInList(list);
			return 1;
		}
		nextEventInList(list);
	}
	headEventInList(list);
	return 0;
}


/**
 * Добавляет событие в список
 * В отличии от похожей функции для списка объектов, не требует ссылку на экземпляр структуры события,
 * а создаёт его автоматически, запрашивая только код события.
 * Не добавляет событие, если оно уже есть
 */
int addEventInList(EventList *list, int event_code)
{
	if(eventInList(list, event_code)) return 0;
	ELE *elem = initELE(initEvent(event_code));
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


/**
 * Удаляет событие с указанным кодом из списка
 */
int delEventFromList(EventList *list, int event_code)
{
	//printf("Deep deleting\n");
	if(event_code != 0) headEventInList(list);
	while(list->current != NULL)
	{
		if((list->current != NULL)&(list->current->event->event_code == event_code))
		{
			if(list->current->prev != NULL) list->current->prev->next = list->current->next;
			if(list->current->next != NULL) list->current->next->prev = list->current->prev;
			if(list->current == list->head) list->head = list->current->next;
			ELE *new_cur = list->current->next;
			free(list->current);
			list->current = new_cur;
		//printf("Deep deleting Done\n");
		}
		if(event_code == 0) return 0;
		if(list->current != NULL) if(list->current->event->event_code != event_code) nextEventInList(list);
	}
	headEventInList(list);
	return 0;
}


/**
 * Удаляет список событий
 */
int delEventList(EventList *list)
{
	headEventInList(list);
	while(list->head != NULL) delEventFromList(list, 0);
	if(list != NULL) free(list);
	return 0;
}


/**
 * Функция для обработки текущих анимаций объектов.
 * Выставляет и пересчитывает анимации объектов в зависимости от контекста
 */
int animationHandler(ObjList *list)
{
	headObjInList(list);
	while(list->current != NULL)
	{
		if(list->current->object->type == TYPE_PLAYER)
		{
			if(eventInList(list->current->object->events, RUN_RIGHT)) updatePlayerRunAnim(list->current->object, 1);
			else if(eventInList(list->current->object->events, RUN_LEFT)) updatePlayerRunAnim(list->current->object, -1);
			else updatePlayerStaticAnim(list->current->object);
		}
		nextObjInList(list);
	}
	headObjInList(list);
	return 0;
}


/**
 * Обрабатывает события, происходящие с объектами
 * Использует для каждого типа объектов соответствующие функции
 */
int eventHandler(ObjList *list)
{
	headObjInList(list);
	while(list->current != NULL)
	{
		if(list->current->object->type == TYPE_PLAYER)
		{
			playerEventHandler(list->current->object);
		}
		nextObjInList(list);
	}
	headObjInList(list);
	return 0;
}


///**
// * Возвращает тип касания двух объектов, если оно есть
// * В противном случае 0
// * Первый аргумент - объект совершающий касание
// * Второй аргумент - объект "принимающий" касание
// */
int touchingCalculator(Obj *donor, Obj *acceptor)
{
	int movx = donor->moving.x;
	int movy = donor->moving.y;
	SDL_Rect *don_box = malloc(sizeof(SDL_Rect));
	donor->box.x -= donor->moving.x;
	donor->box.y -= donor->moving.y;
	don_box->x = donor->box.x + movx;
	don_box->y = donor->box.y + movy;
	SDL_Rect *lacmus = calloc(1, sizeof(SDL_Rect));
	if(SDL_HasIntersection(don_box, &acceptor->box) == SDL_TRUE)
	{
		printf("TOUCH:\n");
		float fx = movx;
		float fy = movy;
		float ky = fy/fx;
		float kx = fx/fy;

		if(fx != 0 )
		{
			while(movx != 0)
			{
				SDL_IntersectRect(don_box, &acceptor->box, lacmus);
				if((lacmus->h == 1)||(lacmus->w <= 2))
				{
					printf("brk\n");
					break;
				}
				movx -= abs(movx)/movx;
				movy = movx*ky;
				don_box->x = donor->box.x + movx;
				don_box->y = donor->box.y + movy;
			}
		}
		if(fy != 0 )
		{
			while(movy != 0)
			{
				SDL_IntersectRect(don_box, &acceptor->box, lacmus);
				if((lacmus->h == 1)||(lacmus->w <= 2))
				{
					printf("brk\n");
					break;
				}
				movy -= abs(movy)/movy;
				movx = movy*kx;
				don_box->y = donor->box.y + movy;
				don_box->x = donor->box.x + movx;
			}
		}
		SDL_IntersectRect(don_box, &acceptor->box, lacmus);
	}
	free(don_box);
	donor->box.x += donor->moving.x;
	donor->box.y += donor->moving.y;
	donor->moving.x = 0;
	donor->moving.y = 0;
	int xltt = acceptor->box.x + 1;
	int xltl = acceptor->box.x;
	int yltt = acceptor->box.y;
	int yltl = acceptor->box.y + 1;
	int xrtr = acceptor->box.x + acceptor->box.w;
	int xrtt = acceptor->box.x + acceptor->box.w - 1;
	int yrtt = acceptor->box.y;
	int yrtr = acceptor->box.y + 1;
	int xlbb = acceptor->box.x + 1;
	int xlbl = acceptor->box.x;
	int ylbb = acceptor->box.y + acceptor->box.h;
	int ylbl = acceptor->box.y + acceptor->box.h - 1;
	int xrbb = acceptor->box.x + acceptor->box.w - 1;
	int xrbr = acceptor->box.x + acceptor->box.w;
	int yrbb = acceptor->box.y + acceptor->box.h;
	int yrbr = acceptor->box.y + acceptor->box.h - 1;
	if(SDL_IntersectRectAndLine(lacmus, &xltl, &yltl, &xlbl, &ylbl) == SDL_TRUE)
	{
		free(lacmus);
		return LEFT_TOUCH;
	}
	if(SDL_IntersectRectAndLine(lacmus, &xltt, &yltt, &xrtt, &yrtt) == SDL_TRUE)
	{
		free(lacmus);
		return TOP_TOUCH;
	}
	if(SDL_IntersectRectAndLine(lacmus, &xrtr, &yrtr, &xrbr, &yrbr) == SDL_TRUE)
	{
		free(lacmus);
		return RIGHT_TOUCH;
	}
	if(SDL_IntersectRectAndLine(lacmus, &xlbb, &ylbb, &xrbb, &yrbb) == SDL_TRUE)
	{
		free(lacmus);
		return BOTTOM_TOUCH;
	}
	return 0;
}

