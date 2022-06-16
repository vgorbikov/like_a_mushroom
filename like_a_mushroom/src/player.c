#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "player.h"

#define PLAYER_RUN_SPEED 500

Obj *initPlayer(SDL_Renderer *rend, int x, int y)
{
	SDL_Rect *pTexBox = malloc(sizeof(SDL_Rect));
	pTexBox->x = 0;
	pTexBox->y = 0;
	pTexBox->w = 100;
	pTexBox->h = 100;
	ObjAnim *playerAnim = initObjAnim(rend, "textures/player_anim.png", pTexBox);
//	printf("Texture OK\n");
	SDL_Rect playerBox;
	playerBox.x = x;
	playerBox.y = y;
	playerBox.h = BLOCK_SIZE;
	playerBox.w = BLOCK_SIZE;
	Obj *player = initObject(playerBox, playerAnim, TYPE_PLAYER);
	printf("Player OK\n");
	return player;
}



/*
 * Анимация бега игрока находится в слотах текстуры на позициях 1-6
 */
void updateRunAnim(Obj *obj, int direction)
{
//	printf("Uodate run anim...\n");
	long int pres_time = clock();
	int frame = 1 + ((pres_time - obj->animation->start_moment)*5/PLAYER_ANIMATION_DURATION)%6;
	obj->animation->tex_box->x = obj->animation->tex_box->w*frame;
	if(direction < 0) obj->animation->flip = SDL_FLIP_HORIZONTAL;
	else obj->animation->flip = SDL_FLIP_NONE;
}


void updateStaticAnim(Obj *obj)
{
//	printf("Uodate static anim...\n");
	obj->animation->tex_box->x = 0;
}


///**
// * Перемещает объект по оси х ()
// */
//void run(Obj *obj, int speed)
//{
//	if(obj->status != RUN_STATUS)
//	{
//		obj->status = RUN_STATUS;
//		obj->x_speed = speed;
//	}
//}

