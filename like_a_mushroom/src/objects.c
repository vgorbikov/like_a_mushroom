#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "objects.h"
#include "menu.h"
#include "map_funcs.h"
#include "player.h"
#include "enemies.h"


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
	new_object->y_speed = 0;
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
	if(obj == NULL) return 1;
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
		if(list->current != NULL) free(list->current);
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
	if(eventInList(list, event_code)) return 1;
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
			if(list->current != NULL) free(list->current);
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
 * Накладывает событие на все объекты в списке
 */
void globalEvent(ObjList *objs, int event_code)
{
	headObjInList(objs);
	while(objs->current != NULL)
	{
		addEventInList(objs->current->object->events, event_code);
		nextObjInList(objs);
	}
	headObjInList(objs);
}

/**
 * Функция для обработки текущих анимаций объектов.
 * Выставляет и пересчитывает анимации объектов в зависимости от контекста
 */
int animationHandler(Map *map)
{
	headObjInList(map->all_obj);
	while(map->all_obj->current != NULL)
	{
		if(map->all_obj->current->object->type == TYPE_PLAYER)
		{
			if(eventInList(map->all_obj->current->object->events, RUN_RIGHT)) updatePlayerRunAnim(map->all_obj->current->object, 1);
			else if(eventInList(map->all_obj->current->object->events, RUN_LEFT)) updatePlayerRunAnim(map->all_obj->current->object, -1);
			else updatePlayerStaticAnim(map->all_obj->current->object);
			if(map->all_obj->current->object->objects_below->head == NULL) updatePlayerJumpAnim(map->all_obj->current->object);
			if(eventInList(map->all_obj->current->object->events, DEATH)) updatePlayerDeathAnim(map->all_obj->current->object);
		}
		if(map->all_obj->current->object->type == TYPE_MARIO)
		{
			if(eventInList(map->all_obj->current->object->events, RUN_RIGHT)) updateMarioRunAnim(map->all_obj->current->object, 1);
			else if(eventInList(map->all_obj->current->object->events, RUN_LEFT)) updateMarioRunAnim(map->all_obj->current->object, -1);
			else updateMarioStaticAnim(map->all_obj->current->object);
		}
		nextObjInList(map->all_obj);
	}
	headObjInList(map->all_obj);
	return 0;
}


/**
 * Обрабатывает события, происходящие с объектами
 * Использует для каждого типа объектов соответствующие функции
 */
int eventHandler(Map *map)
{
	headObjInList(map->all_obj);
	while(map->all_obj->current != NULL)
	{
		if(map->all_obj->current->object->type == TYPE_PLAYER)
		{
			playerEventHandler(map->all_obj->current->object);
		}
		if(map->all_obj->current->object->type == TYPE_MARIO)
		{
			marioEventHandler(map->all_obj->current->object);
		}
		nextObjInList(map->all_obj);
	}
	headObjInList(map->all_obj);
	return 0;
}


/**
 * Перемещает объект вниз с ускорением
 */
int gravitation(Obj *obj)
{
	long int time = clock();
	if(obj->objects_below->head != NULL) obj->events->current->event->start_moment = time;
	obj->y_speed += G*(time - obj->events->current->event->start_moment)/1000;
	return 0;
}



/**
 * Пока действует, перемещает объект с присущей ему скоростью
 * Для "живых" объектов
 */
int Run(Obj *somebody, int direction)
{
	ObjList *left = somebody->objects_left;
	ObjList *right = somebody->objects_right;
	if((left->head != NULL)&(direction < 0)) return 0;
	if((right->head != NULL)&(direction > 0)) return 0;
	somebody->x_speed += direction;
	return 0;
}


/**
 * Придаёт скорость по оси y в отрицательном направлении
 */
int Jump(Obj *somebody, int speed)
{
//	printf("JUMP!!!\n");
	somebody->y_speed -= speed;
	return 0;
}

/**
 * Вычисляет кооректировку положения объекта после касания
 * Передаёт касание на обработку инициатору событий-касаний
 */
int *Touch(Obj *somebody, Obj* monolith, int touch_code)
{
//	printf("strt tch\n");
	int *correct = calloc(2, sizeof(int));
	int dx = 0;
	int dy = 0;
	switch(touch_code)
	{
		case TOP_TOUCH:
			dy = monolith->box.y - somebody->box.y - somebody->box.h;
			if(!objInList(somebody->objects_below, monolith))
			{
				addObjInList(somebody->objects_below, monolith);
//				printf("top touch obj: %i\n", monolith);
			}
			delEventFromList(somebody->events, JUMP);
			break;
		case LEFT_TOUCH:
			dx = monolith->box.x - somebody->box.x - somebody->box.w;
			if(!objInList(somebody->objects_right, monolith))
			{
				addObjInList(somebody->objects_right, monolith);
//				printf("left touch obj: %i\n", monolith);
			}
			break;
		case RIGHT_TOUCH:
			dx = monolith->box.w - somebody->box.x + monolith->box.x;
			if(!objInList(somebody->objects_left, monolith))
			{
				addObjInList(somebody->objects_left, monolith);
//				printf("right touch obj: %i\n", monolith);
			}
			break;
		case BOTTOM_TOUCH:
			dy = monolith->box.h - somebody->box.y + monolith->box.y;
			if(!objInList(somebody->objects_over, monolith))
			{
				addObjInList(somebody->objects_over, monolith);
//				printf("bottom touch obj: %i\n", monolith);
			}
			delEventFromList(somebody->events, JUMP);
			break;
	}
//	if((dx != 0)||(dy != 0)) printf("%i, %i\n", dx, dy);
	correct[0] = dx;
	correct[1] = dy;
//	if((correct[1]!=0)||(correct[0]!=0)) printf("correct: %i, %i\n", correct[0], correct[1]);
//	printf("end tch\n");
	if(somebody->type == TYPE_PLAYER) playerTouch(somebody, monolith, touch_code);
	return correct;
}



/**
 * Проверяет, касается ли один объект другого до сих пор
 */
void nearbyCalculator(ObjList *objs)
{
	headObjInList(objs);
	while(objs->current != NULL)
	{
		Obj *obj = objs->current->object;
		if(eventInList(obj->events, DEATH))
		{
			nextObjInList(objs);
			continue;
		}
		ObjList *below = obj->objects_below;
		ObjList *left = obj->objects_left;
		ObjList *right = obj->objects_right;
		ObjList *over = obj->objects_over;
	//	int count_b;
		headObjInList(below);
		while(below->current != NULL)
		{
	//		count_b += 1;
	//		printf("below x_code: %i\n", below->current->object->box.x);
			if(!((below->current->object->box.y == obj->box.y + obj->box.h)&(below->current->object->box.x > obj->box.x - below->current->object->box.w)&
					(below->current->object->box.x < obj->box.x + obj->box.w))) delObjFromList(below);
			else nextObjInList(below);

		}
		headObjInList(below);
		headObjInList(left);
		while(left->current != NULL)
		{
	//		printf("below x_code: %i\n", below->current->object->box.x);
			if(!((left->current->object->box.x == obj->box.x - left->current->object->box.w)&(left->current->object->box.y + left->current->object->box.h > obj->box.y)&
					(left->current->object->box.y < obj->box.y + obj->box.h))) delObjFromList(left);
			else nextObjInList(left);

		}
		headObjInList(left);
		headObjInList(right);
		while(right->current != NULL)
		{
	//		printf("below x_code: %i\n", below->current->object->box.x);
			if(!((right->current->object->box.x == obj->box.x + obj->box.w)&(right->current->object->box.y + right->current->object->box.h > obj->box.y)&
					(right->current->object->box.y < obj->box.y + obj->box.h))) delObjFromList(right);
			else nextObjInList(right);

		}
		headObjInList(right);
		headObjInList(over);
		while(over->current != NULL)
		{
	//		count_b += 1;
	//		printf("below x_code: %i\n", below->current->object->box.x);
			if(!((over->current->object->box.y + over->current->object->box.h == obj->box.y)&(over->current->object->box.x > obj->box.x - over->current->object->box.w)&
					(over->current->object->box.x < obj->box.x + obj->box.w))) delObjFromList(over);
			else nextObjInList(over);

		}
		headObjInList(over);
	//	printf("Over count: %i\n", count_b);
		nextObjInList(objs);
	}
}



/**
 * Обнуляет перемещение всех объектов из списка
 * Следует применять в конце каждого цикла отрисовки
 */
void movingClear(Map *map)
{
	headObjInList(map->movable_obj);
	while(map->movable_obj->current != NULL)
	{
		map->movable_obj->current->object->moving.x = 0;
		map->movable_obj->current->object->moving.y = 0;
		map->movable_obj->current->object->x_speed = 0;
		map->movable_obj->current->object->y_speed = 0;
		nextObjInList(map->movable_obj);
	}
	headObjInList(map->movable_obj);
}

/**
 * Обновляет время последнего перемещения для всех объектов в спискее
 * Применяется перед запуском игрового цикла для корректной обработки начального положения объектов
 */
void setLastMoveTime(ObjList *objs)
{
	headObjInList(objs);
	while(objs->current != NULL)
	{
		long int now = clock();
		objs->current->object->last_xmove = now;
		objs->current->object->last_ymove = now;
		objs->current->object->last_yspeed_upd = now;
		nextObjInList(objs);
	}
	headObjInList(objs);
}


/**
 * Придаёт объекту перемещение исходя из его скорости в данный момент времени
 */
int movingCalculator(ObjList *objs)
{
	headObjInList(objs);
	while(objs->current != NULL)
	{
		long int time = clock();
		Obj *obj = objs->current->object;
		float tempydt = (time - obj->last_ymove);
		float ydt = tempydt/1000;
		if(!(((obj->y_speed*ydt) < 1) & (obj->y_speed*ydt > -1) & (obj->y_speed != 0)))
		{
			obj->moving.y += obj->y_speed*ydt;
			obj->last_ymove = clock();
		}
		else obj->moving.y += 0;

		float tempxdt = (time - obj->last_xmove);
		float xdt = tempxdt/1000;
		if(!(((obj->x_speed*xdt) < 1) & (obj->x_speed*xdt > -1) & (obj->x_speed != 0)))
		{
			obj->moving.x += obj->x_speed*xdt;
			obj->last_xmove = clock();
		}
		else obj->moving.x += 0;

		if((obj->objects_below->head == NULL)&(obj->moving.y > 0)) obj->box.y += obj->moving.y;
		if((obj->objects_over->head == NULL)&(obj->moving.y < 0)) obj->box.y += obj->moving.y;
		if((obj->objects_left->head == NULL)&(obj->moving.x < 0)) obj->box.x += obj->moving.x;
		if((obj->objects_right->head == NULL)&(obj->moving.x > 0)) obj->box.x += obj->moving.x;

		nextObjInList(objs);
	}
	return 0;
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


float getDistance(SDL_Rect *box1, SDL_Rect *box2)
{
	return sqrt(pow(box1->x - box2->x, 2) + pow(box1->y - box2->y, 2));
}


/**
 * Возвращает тип касания двух объектов, если оно есть
 * В противном случае 0
 * Первый аргумент - объект совершающий касание
 * Второй аргумент - объект "принимающий" касание
 */
int touchingCalculator(Obj *donor, Obj *acceptor)
{
//	printf("start calcul\n");
	int code = 0;
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
		if(hasIntersectTwoSegment(tracking_point_before, tracking_point, tl, bl)) code = LEFT_TOUCH;
		if(hasIntersectTwoSegment(tracking_point_before, tracking_point, tl, tr)) code = TOP_TOUCH;
		if(hasIntersectTwoSegment(tracking_point_before, tracking_point, tr, br)) code = RIGHT_TOUCH;
		if(hasIntersectTwoSegment(tracking_point_before, tracking_point, bl, br)) code = BOTTOM_TOUCH;
		free(tl);
		free(tr);
		free(bl);
		free(br);
	}

	if(((acceptor->box.y == donor->box.y + donor->box.h)&(acceptor->box.x > donor->box.x - acceptor->box.w)&
			(acceptor->box.x < donor->box.x + donor->box.w))) code = TOP_TOUCH;

	else if(((acceptor->box.x == donor->box.x - acceptor->box.w)&(acceptor->box.y + acceptor->box.h > donor->box.y)&
			(acceptor->box.y < donor->box.y + donor->box.h))) code = RIGHT_TOUCH;

	else if(((acceptor->box.x == donor->box.x + donor->box.w)&(acceptor->box.y + acceptor->box.h > donor->box.y)&
			(acceptor->box.y < donor->box.y + donor->box.h))) code = LEFT_TOUCH;

	else if(((acceptor->box.y + acceptor->box.h == donor->box.y)&(acceptor->box.x > donor->box.x - acceptor->box.w)&
					(acceptor->box.x < donor->box.x + donor->box.w))) code = BOTTOM_TOUCH;
//	printf("done calcul\n");
	return code;
}


/**
 * Принимает объект карты и просчитывает касания движущихся объектов
 */
void touchingHandler(Map *map)
{
	headObjInList(map->movable_obj);
	while(map->movable_obj->current != NULL)
	{
		Obj *someone = map->movable_obj->current->object;
		if(eventInList(someone->events, DEATH))
		{
			nextObjInList(map->movable_obj);
			continue;
		}
		int dx = 0;
		int dy = 0;
		headObjInList(map->all_obj);
		while(map->all_obj->current != NULL)
		{
			Obj *cur = map->all_obj->current->object;

			//не просчитываем касания для слишком отдалённых друг от друга предметов
			if((cur != someone)&(getDistance(&someone->box, &cur->box) < 3*BLOCK_SIZE)&(!eventInList(cur->events, DEATH)))
			{
				int *correct = Touch(someone, cur, touchingCalculator(someone, cur));
				if((correct[1]!=0)||(correct[0]!=0)) printf("correct: (%i, %i)\n", correct[0], correct[1]);

				if(((correct[0] > dx)&(dx>0))||((correct[0] < dx)&(dx<0))||(dx == 0)) dx = correct[0];
				if(((correct[1] > dy)&(dy>0))||((correct[1] < dy)&(dy<0))||(dy == 0)) dy = correct[1];

				if(correct != NULL) free(correct);


			}
			nextObjInList(map->all_obj);
		}
		if((dx!=0)||(dy!=0)) printf("totally correct: (%i, %i)\n", dx, dy);
		someone->box.x += dx;
		someone->box.y += dy;
		nextObjInList(map->movable_obj);
	}
}


