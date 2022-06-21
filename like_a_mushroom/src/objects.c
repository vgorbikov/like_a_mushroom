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
	Obj *new_object = calloc(1, sizeof(Obj));
	new_object->box = obj_box;
	new_object->x_speed = 0;
	new_object->last_yspeed_upd = 0;
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
			if(list->current->object->objects_below->head == NULL) updatePlayerJumpAnim(list->current->object);
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


void movingClear(ObjList *list)
{
	headObjInList(list);
	while(list->current != NULL)
	{
		list->current->object->moving.x = 0;
		list->current->object->moving.y = 0;
		nextObjInList(list);
	}
	headObjInList(list);
}


/**
 * Возвращает 1, если отрезки пересекаются
 * В противном случае 0
 */
int hasIntersectTwoSegment(SDL_Point *begin1, SDL_Point *end1, SDL_Point *begin2, SDL_Point *end2)
{
	float x = 0;//точка пересечения
	float y = 0;
	float dx1 = end1->x - begin1->x;
	float dy1 = end1->y - begin1->y;
	float dx2 = end2->x - begin2->x;
	float dy2 = end2->y - begin2->y;
	float k1 = dy1/dx1;
	float k2 = dy2/dx2;
	if((dx1 == 0)&(dx2 == 0)) //если обе линии вертикальны
	{
//		printf("Vert&Vert\n");
		if(begin1->x != begin2->x) return 0; //не пересекаются
		if(((begin1->y < begin2->y)&(begin1->y < end2->y)&(end1->y < begin2->y)&(end1->y < end2->y))||
				((begin1->y > begin2->y)&(begin1->y > end2->y)&(end1->y > begin2->y)&(end1->y > end2->y))) return 0;
		else return 1;

	}
	if((k1 == 0)&(k2 == 0)) //если обе линии горизонтальны
	{
//		printf("Hor&Hor\n");
		if(begin1->y != begin2->y) return 0; //не пересекаются
		if(((begin1->x < begin2->x)&(begin1->x < end2->x)&(end1->x < begin2->x)&(end1->x < end2->x))||
				((begin1->x > begin2->x)&(begin1->x > end2->x)&(end1->x > begin2->x)&(end1->x > end2->x))) return 0;
		else return 1;

	}
	if(dx1 == 0) //если вертикальна только первая линия
	{
//		printf("Ver&...\n");
		x = begin1->x;
		y = k2*(x - begin2->x) + begin2->y;
	}
	if(dx2 == 0) //если вертикальна только вторая линия
	{
//		printf("...&Vert\n");
		x = begin2->x;
		y = k1*(x - begin1->x) + begin1->y;
	}
	if(k1 == 0) //если горизонтальна только первая линия
	{
//		printf("Hor&...\n");
		y = begin1->y;
		x = (y - begin2->y + k2*begin2->x)/k2;
	}
	if(k2 == 0) //если горизонтальна только вторая линия
	{
//		printf("...&Hor\n");
		y = begin2->y;
		x = (y - begin1->y + k1*begin1->x)/k1;
	}
	if((dx1 == 0)&(k2==0)) //первая линия вертикальна, вторая горизонтальна
	{
		x = begin1->x;
		y = begin2->y;
	}
	if((dx2 == 0)&(k1==0)) //первая линия горизонтальна, вторая вертикальна
	{
		x = begin2->x;
		y = begin1->y;
	}
	//случай с двумя наклонными линиями обрабатывать пока не требуется
	int belong_x1 = ((x <= begin1->x)&(x >= end1->x))||((x >= begin1->x)&(x <= end1->x));
	int belong_x2 = ((x <= begin2->x)&(x >= end2->x))||((x >= begin2->x)&(x <= end2->x));
	int belong_y1 = ((y <= begin1->y)&(y >= end1->y))||((y >= begin1->y)&(y <= end1->y));
	int belong_y2 = ((y <= begin2->y)&(y >= end2->y))||((y >= begin2->y)&(y <= end2->y));
	int belong_1 = belong_x1 & belong_y1;
	int belong_2 = belong_x2 & belong_y2;
	if(belong_1 & belong_2)
	{
//		printf("OK\n");
		return 1;
	}
	else return 0;

	printf("Bullshit occured in hasIntersectionTwoSegment()\n");
	return 0;
}


/**
 * Возвращает тип касания двух объектов, если оно есть
 * В противном случае 0
 * Первый аргумент - объект совершающий касание
 * Второй аргумент - объект "принимающий" касание
 */
int touchingCalculator(Obj *donor, Obj *acceptor)
{
	if(SDL_HasIntersection(&donor->box, &acceptor->box) == SDL_TRUE) //если есть пересечение, нужно понять с какой стороны оно пришло
	{
		SDL_Rect *intersect = calloc(1, sizeof(SDL_Rect)); //наче получаем пересечение
		SDL_IntersectRect(&donor->box, &acceptor->box, intersect);

		//далее выбираем на границе пересечения точку, перемещение которой будем отслеживать
		SDL_Point *tracking_point = calloc(1, sizeof(SDL_Point));
		int dx = donor->moving.x;
		int dy = donor->moving.y;
		//	printf("moving = (%i, %i)\n", dx, dy);
		if((dy == 0)&(dx > 0))
		{
		//		printf("right center point\n");
			tracking_point->x = intersect->x + intersect->w;
			tracking_point->y = intersect->y + intersect->h/2;
		}
		if((dy > 0)&(dx > 0))
		{
		//		printf("left top point\n");
			tracking_point->x = intersect->x + intersect->w;
			tracking_point->y = intersect->y + intersect->h;
		}
		if((dy > 0)&(dx == 0))
		{
		//		printf("top center point\n");
			tracking_point->x = intersect->x + intersect->w/2;
			tracking_point->y = intersect->y + intersect->h;
		}
		if((dy > 0)&(dx < 0))
		{
		//		printf("right top point\n");
			tracking_point->x = intersect->x;
			tracking_point->y = intersect->y + intersect->h;
		}
		if((dy == 0)&(dx < 0))
		{
		//		printf("left center point\n");
			tracking_point->x = intersect->x;
			tracking_point->y = intersect->y + intersect->h/2;
		}
		if((dy < 0)&(dx < 0))
		{
		//		printf("left bottom point\n");
			tracking_point->x = intersect->x;
			tracking_point->y = intersect->y;
		}
		if((dy < 0)&(dx == 0))
		{
		//		printf("bottom center point\n");
			tracking_point->x = intersect->x + intersect->w/2;
			tracking_point->y = intersect->y;
		}
		if((dy < 0)&(dx > 0))
		{
		//		printf("left bottom point\n");
			tracking_point->x = intersect->x + intersect->w;
			tracking_point->y = intersect->y;
		}
		free(intersect);
		SDL_Point *tracking_point_before = malloc(sizeof(SDL_Point));
		tracking_point_before->x = tracking_point->x - donor->moving.x;
		tracking_point_before->y = tracking_point->y - donor->moving.y;
		//точки коробки акцептора
		SDL_Point *tl = malloc(sizeof(SDL_Point)); //верхний левый угол
		SDL_Point *tr = malloc(sizeof(SDL_Point)); //верхний правый
		SDL_Point *bl = malloc(sizeof(SDL_Point)); //нижний левый
		SDL_Point *br = malloc(sizeof(SDL_Point)); //нижний правый
		tl->x = acceptor->box.x;
		tl->y = acceptor->box.y;
		tr->x = acceptor->box.x + acceptor->box.w;
		tr->y = acceptor->box.y;
		bl->x = acceptor->box.x;
		bl->y = acceptor->box.y + acceptor->box.h;
		br->x = acceptor->box.x + acceptor->box.w;
		br->y = acceptor->box.y + acceptor->box.h;
		if(hasIntersectTwoSegment(tracking_point_before, tracking_point, tl, bl)) return LEFT_TOUCH;
		if(hasIntersectTwoSegment(tracking_point_before, tracking_point, tl, tr)) return TOP_TOUCH;
		if(hasIntersectTwoSegment(tracking_point_before, tracking_point, tr, br)) return RIGHT_TOUCH;
		if(hasIntersectTwoSegment(tracking_point_before, tracking_point, bl, br)) return BOTTOM_TOUCH;
	}

	if(((acceptor->box.y == donor->box.y + donor->box.h)&(acceptor->box.x > donor->box.x - acceptor->box.w)&
			(acceptor->box.x < donor->box.x + donor->box.w))) return TOP_TOUCH;

	if(((acceptor->box.x == donor->box.x - acceptor->box.w)&(acceptor->box.y + acceptor->box.h > donor->box.y)&
			(acceptor->box.y < donor->box.y + donor->box.h))) return RIGHT_TOUCH;

	if(((acceptor->box.x == donor->box.x + donor->box.w)&(acceptor->box.y + acceptor->box.h > donor->box.y)&
			(acceptor->box.y < donor->box.y + donor->box.h))) return LEFT_TOUCH;

	return 0;
}


///**
// * Принимает список объектов на карте и просчитывает их касания с игроком
// */
//void touchingHandler(ObjList *objlist, Obj *player)
//{
//	headObjInList(objlist);
//	while(objlist->current != NULL)
//	{
//		Obj *cur = objlist->current->object;
//
//		nextObjInList(objlist);
//	}
//
//
//}
