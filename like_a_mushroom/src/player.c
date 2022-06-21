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
	player->objects_below = initObjList();
	player->objects_right = initObjList();
	player->objects_left = initObjList();
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


void updatePlayerJumpAnim(Obj *obj)
{
	if(obj->animation->status != JUMP_STATUS)
	{
		obj->animation->start_moment= clock();
		obj->animation->status = JUMP_STATUS;
	}
	obj->animation->tex_box->x = 700;
}


/**
 * Пока действует, перемещает объект с присущей ему скоростью
 */
int playerRun(Obj *player, int direction)
{
	ObjList *left = player->objects_left;
	ObjList *right = player->objects_right;
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
	if((left->head != NULL)&(direction < 0)) return 0;
	if((right->head != NULL)&(direction > 0)) return 0;
	player->box.x += player->moving.x;
	return 0;
}


int playerJump(Obj *player)
{
	printf("JUMP!!!\n");
	long int time = clock();
	if(abs(time - player->events->current->event->start_moment) <= 10) player->last_ymove = player->events->current->event->start_moment;
	float tempdt = (time - player->last_ymove);
	float dt = tempdt/1000;
	if(!(((PLAYER_JUMP_SPEED*dt) < 1) & (PLAYER_JUMP_SPEED*dt > -1)))
	{
		player->moving.y += PLAYER_JUMP_SPEED*dt*(-1);
		player->last_ymove = clock();
	}
	else player->moving.y += 0;
	player->box.y += player->moving.y;
	return 0;
}


int *playerTouchMonolith(Obj *player, Obj* monolith, int touch_code)
{
	int *correct = calloc(2, sizeof(int));
	int dx = 0;
	int dy = 0;
	switch(touch_code)
	{
		case TOP_TOUCH:
			dy = monolith->box.y - player->box.y - player->box.h;
			if(!objInList(player->objects_below, monolith))
			{
				addObjInList(player->objects_below, monolith);
				printf("top touch obj: %i\n", monolith);
			}
			delEventFromList(player->events, JUMP);
			break;
		case LEFT_TOUCH:
			dx = monolith->box.x - player->box.x - player->box.w;
			if(!objInList(player->objects_right, monolith))
			{
				addObjInList(player->objects_right, monolith);
				printf("left touch obj: %i\n", monolith);
			}
			break;
		case RIGHT_TOUCH:
			dx = monolith->box.w - player->box.x + monolith->box.x;
			if(!objInList(player->objects_left, monolith))
			{
				addObjInList(player->objects_left, monolith);
				printf("right touch obj: %i\n", monolith);
			}
			break;
		case BOTTOM_TOUCH:
			printf("bottom touch obj: %i\n", monolith);
			dy = monolith->box.h - player->box.y + monolith->box.y;
			break;
	}
	correct[1] = dx;
	correct[2] = dy;
	return correct;
}


/**
 * Перемещает объект вниз с ускорением
 */
int gravitation(Obj *obj)
{
	ObjList *below = obj->objects_below;
	long int time = clock();
	if(abs(time - obj->events->current->event->start_moment) <= 10)
	{
		obj->last_yspeed_upd = obj->events->current->event->start_moment;
		obj->last_ymove = obj->events->current->event->start_moment;
	}
	float tempdt = (time - obj->last_ymove);
	float dt = tempdt/1000;
	float y_speed = G*(time - obj->last_yspeed_upd)/1000;
	if(!(((y_speed*dt) < 1) & (y_speed*dt > -1) & (y_speed != 0)))
	{
		obj->moving.y += y_speed*dt;
		obj->last_ymove = clock();
	}
	else obj->moving.y += 0;
	if(below->head != NULL)
	{
		obj->last_yspeed_upd = time;
		return 0; //не применяем гравитацию к объектам, стоящим на монолитных блоках
	}
	obj->box.y += obj->moving.y;
	return 0;
}


/**
 * Проверяет, касается ли один объект другого до сих пор
 */
void nearbyCalculator(Obj *obj)
{
	ObjList *below = obj->objects_below;
	ObjList *left = obj->objects_left;
	ObjList *right = obj->objects_right;
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
//	printf("Below count: %i\n", count_b);
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
		if(event->event_code == JUMP)  playerJump(player);
		nextEventInList(player->events);
	}
	headEventInList(player->events);
	return 0;
}
