#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "objects.h"
#include "player.h"
#include "status_bar.h"

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
	player->x_speed = 0;
	player->objects_below = initObjList();
	player->objects_right = initObjList();
	player->objects_left = initObjList();
	player->objects_over = initObjList();
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


void playerDeath(Obj *player)
{

}


int playerTouch(Obj *player, Obj* obj)
{

	return 0;
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
		if(event->event_code == RUN_RIGHT)  Run(player, PLAYER_RUN_SPEED);
		if(event->event_code == RUN_LEFT)  Run(player, -PLAYER_RUN_SPEED);
		if(event->event_code == JUMP)  Jump(player);
		if(event->event_code == GRAVITATION)  gravitation(player);
		nextEventInList(player->events);
	}
	headEventInList(player->events);
	return 0;
}
