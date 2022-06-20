#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "player.h"


/**
 * Возвращает объект типа "TYPE_PLAYER"
 */
Obj *initPlayer(SDL_Renderer *rend, int x, int y)
{
	SDL_Rect *pTexBox = malloc(sizeof(SDL_Rect));
	pTexBox->x = 0;
	pTexBox->y = 0;
	pTexBox->w = 100;
	pTexBox->h = 100;
	ObjAnim *playerAnim = initObjAnim(rend, PLAYER_TEXTURE, pTexBox);
//	printf("Texture OK\n");
	SDL_Rect playerBox;
	playerBox.x = x;
	playerBox.y = y;
	playerBox.h = BLOCK_SIZE;
	playerBox.w = BLOCK_SIZE;
	Obj *player = initObject(playerBox, playerAnim, TYPE_PLAYER);
	player->x_speed = PLAYER_RUN_SPEED;
	player->y_speed = FALL_SPEED;
	player->objects_below = initObjList();
//	printf("Player OK\n");
	return player;
}



/*
 * Обновляет (или устанавливает) для игрока анимацию бега в указанном направлении
 * Анимация бега игрока находится в слотах текстуры на позициях 1-6
 */
void updatePlayerRunAnim(Obj *obj, int direction)
{
//	printf("Uodate run anim...\n");
	if(obj->animation->status != RUN_STATUS)
	{
		obj->animation->start_moment= clock();
		obj->animation->status = RUN_STATUS;
	}
	long int pres_time = clock();
	int frame = 1 + ((pres_time - obj->animation->start_moment)*5/PLAYER_RUN_ANIMATION_DURATION)%6;
	obj->animation->tex_box->x = obj->animation->tex_box->w*frame;
	if(direction < 0) obj->animation->flip = SDL_FLIP_HORIZONTAL;
	else obj->animation->flip = SDL_FLIP_NONE;
}


/**
 * Устанавливает для игрока статическую анимацию
 */
void updatePlayerStaticAnim(Obj *obj)
{
//	printf("Uodate static anim...\n");
	if(obj->animation->status != STATIC_STATUS)
	{
		obj->animation->start_moment= clock();
		obj->animation->status = STATIC_STATUS;
	}
	obj->animation->tex_box->x = 0;
}


void updatePlayerJumpAnim(Obj *obj, int direction)
{
	if(obj->animation->status != JUMP_STATUS)
	{
		obj->animation->start_moment= clock();
		obj->animation->status = JUMP_STATUS;
	}
	obj->animation->tex_box->x = 700;
	if(direction < 0) obj->animation->flip = SDL_FLIP_HORIZONTAL;
	else obj->animation->flip = SDL_FLIP_NONE;
}


/**
 * Пока действует, перемещает объект с присущей ему скоростью
 */
int playerRun(Obj *player, int direction)
{
	long int time = clock();
	if(abs(time - player->events->current->event->start_moment) <= 10) player->last_xmove = player->events->current->event->start_moment;
	float tempxdt = (time - player->last_xmove);
	float xdt = tempxdt/1000;
	if(!(((player->x_speed*xdt*direction) < 1) & (player->x_speed*xdt*direction > -1) & (player->x_speed != 0)))
	{
		player->moving.x = player->x_speed*xdt*direction;
		player->last_xmove = clock();
	}
	else player->moving.x = 0;
	player->box.x += player->moving.x;
	return 0;
}


void playerTouchMonolith(Obj *player, Obj* monolith, int touch_code)
{
	switch(touch_code)
	{
		case TOP_TOUCH:
//			printf("top touch obj: %i\n", monolith);
			player->box.y = monolith->box.y - player->box.h;
			addObjInList(player->objects_below, monolith);
			break;
		case LEFT_TOUCH:
//			printf("left touch obj: %i\n", monolith);
			player->box.x = monolith->box.x - player->box.w;
			break;
		case RIGHT_TOUCH:
//			printf("right touch obj: %i\n", monolith);
			player->box.x = monolith->box.x + monolith->box.w;
			break;
		case BOTTOM_TOUCH:
//			printf("bottom touch obj: %i\n", monolith);
			player->box.y = monolith->box.y + monolith->box.h;
			break;
	}
}


/**
 * Перемещает объект вниз с ускорением
 */
int gravitation(Obj *obj)
{
	ObjList *below = obj->objects_below;
	long int time = clock();
	if(abs(time - obj->events->current->event->start_moment) <= 10) obj->last_ymove = obj->events->current->event->start_moment;
	float tempxdt = (time - obj->last_ymove);
	float xdt = tempxdt/1000;
	if(!(((obj->y_speed*xdt) < 1) & (obj->y_speed*xdt > -1) & (obj->y_speed != 0)))
	{
		obj->moving.y = obj->y_speed*xdt;
		obj->last_ymove = clock();
	}
	else obj->moving.y = 0;
	if(below->head != NULL) return 0; //не применяем гравитацию к объектам, стоящим на монолитных блоках
	obj->box.y += obj->moving.y;
	return 0;
}


/**
 * Проверяет, касается ли один объект другого до сих пор
 */
void belowCalculator(Obj *obj)
{
	ObjList *below = obj->objects_below;
	headObjInList(below);
	while(below->current != NULL)
	{
//		printf("below x_code: %i\n", below->current->object->box.x);
		if(!((below->current->object->box.y == obj->box.y + obj->box.h)&(below->current->object->box.x > obj->box.x - below->current->object->box.w)&
				(below->current->object->box.x < obj->box.x + obj->box.w))) delObjFromList(below);
		else nextObjInList(below);

	}
	headObjInList(below);
}


/**
 * Обработчик событий для объектов типа TYPE_PLAYER
 */
int playerEventHandler(Obj *player)
{
	headEventInList(player->events);
	while(player->events->current != NULL)
	{
		ObjEvent *event = player->events->current->event;
		if(event->event_code == RUN_RIGHT)  playerRun(player, 1);
		if(event->event_code == RUN_LEFT)  playerRun(player, -1);
		if(event->event_code == GRAVITATION)  gravitation(player);
		nextEventInList(player->events);
	}
	headEventInList(player->events);
	return 0;
}
