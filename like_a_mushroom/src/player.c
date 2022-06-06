#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "player.h"


/*
 * Анимация бега игрока находится в слотах текстуры на позициях 1-6
 */
void updateRunAnim(Obj *obj)
{
	long int pres_time = clock();
	int frame = 1 + ((pres_time - obj->animation->start_moment)*5/400)%6;
	obj->animation->tex_box->x = obj->animation->tex_box->w*frame;
}

void updateStaticAnim(Obj *obj)
{
	obj->animation->tex_box->x = 0;
}
