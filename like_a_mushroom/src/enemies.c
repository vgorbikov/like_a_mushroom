#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "enemies.h"


/**
 * BULLSHIT
 */
Obj *initMario(SDL_Renderer *rend, int x, int y)
{
	SDL_Rect *mTexBox = malloc(sizeof(SDL_Rect));
	mTexBox->x = 0;
	mTexBox->y = 0;
	mTexBox->w = 100;
	mTexBox->h = 100;
	ObjAnim *marioAnim = initObjAnim(rend, MARIO_TEXTURE, mTexBox);
//	printf("Mario Texture OK\n");
	SDL_Rect marioBox;
	marioBox.x = x;
	marioBox.y = y;
	marioBox.h = BLOCK_SIZE;
	marioBox.w = BLOCK_SIZE;
	Obj *mario = initObject(marioBox, marioAnim, TYPE_MARIO);
	mario->x_speed = 0;
	mario->objects_below = initObjList();
	mario->objects_right = initObjList();
	mario->objects_left = initObjList();
	mario->objects_over = initObjList();
//	printf("Mario OK\n");
	return mario;
}


/*
 * Обновляет (или устанавливает) для игрока анимацию бега в указанном направлении
 * Анимация бега игрока находится в слотах текстуры на позициях 1-6
 */
void updateMarioRunAnim(Obj *obj, int direction)
{
//	printf("Uodate run anim...\n");
	if(obj->animation->status != RUN_STATUS)
	{
		obj->animation->start_moment= clock();
		obj->animation->status = RUN_STATUS;
	}
	long int pres_time = clock();
	int frame = 1 + ((pres_time - obj->animation->start_moment)*5/MARIO_RUN_ANIMATION_DURATION)%6;
	obj->animation->tex_box->x = (obj->animation->tex_box->w)*frame;
	if(direction < 0) obj->animation->flip = SDL_FLIP_HORIZONTAL;
	else obj->animation->flip = SDL_FLIP_NONE;
}


/**
 * Устанавливает для игрока статическую анимацию
 */
void updateMarioStaticAnim(Obj *obj)
{
//	printf("Uodate static anim...\n");
	if(obj->animation->status != STATIC_STATUS)
	{
		obj->animation->start_moment= clock();
		obj->animation->status = STATIC_STATUS;
	}
	obj->animation->tex_box->x = 0;
}


void updateMarioJumpAnim(Obj *obj)
{
	if(obj->animation->status != JUMP_STATUS)
	{
		obj->animation->start_moment= clock();
		obj->animation->status = JUMP_STATUS;
	}
	obj->animation->tex_box->x = 700;
}


/**
 * Обработчик событий для объектов типа TYPE_PLAYER
 */
int marioEventHandler(Obj *mario)
{
	headEventInList(mario->events);
	while(mario->events->current != NULL)
	{
		ObjEvent *event = mario->events->current->event;
		if(event->event_code == RUN_RIGHT)  Run(mario, MARIO_RUN_SPEED);
		if(event->event_code == RUN_LEFT)  Run(mario, -MARIO_RUN_SPEED);
		if(event->event_code == JUMP)  Jump(mario, MARIO_JUMP_SPEED);
		if(event->event_code == GRAVITATION)  gravitation(mario);
		nextEventInList(mario->events);
	}
	headEventInList(mario->events);
	return 0;
}


void enemiesControl(ObjList *ene)
{

	headObjInList(ene);
	while(ene->current != NULL)
	{
		Obj *cur_ene = ene->current->object;
		if((cur_ene->objects_left->head == NULL)&!(eventInList(cur_ene->events, RUN_RIGHT))) addEventInList(cur_ene->events, RUN_LEFT);
		else if((cur_ene->objects_right->head == NULL)&!(eventInList(cur_ene->events, RUN_LEFT))) addEventInList(cur_ene->events, RUN_RIGHT);
		if((cur_ene->objects_left->head != NULL)&(eventInList(cur_ene->events, RUN_LEFT))) delEventFromList(cur_ene->events, RUN_LEFT);
		if((cur_ene->objects_right->head != NULL)&(eventInList(cur_ene->events, RUN_RIGHT))) delEventFromList(cur_ene->events, RUN_RIGHT);
		nextObjInList(ene);
	}
	headObjInList(ene);
}

