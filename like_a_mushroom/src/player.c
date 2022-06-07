#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "player.h"


void run(Obj *obj, int direction)
{
	obj->x_speed = direction;
}


/*
 * Анимация бега игрока находится в слотах текстуры на позициях 1-6
 */
void updateRunAnim(Obj *obj)
{
	long int pres_time = clock();
	int frame = 1 + ((pres_time - obj->animation->start_moment)*5/PLAYER_ANIMATION_DURATION)%6;
	obj->animation->tex_box->x = obj->animation->tex_box->w*frame;
	if(obj->x_speed < 0) obj->animation->flip = SDL_FLIP_HORIZONTAL;
	else obj->animation->flip = SDL_FLIP_NONE;
}

void updateStaticAnim(Obj *obj)
{
	obj->animation->tex_box->x = 0;
}
