#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "objects.h"
#include "enemies.h"

Obj *initMario(SDL_Renderer *rend, int x, int y)
{
	SDL_Rect *mTexBox = malloc(sizeof(SDL_Rect));
	mTexBox->x = 0;
	mTexBox->y = 0;
	mTexBox->w = 100;
	mTexBox->h = 100;
	ObjAnim *marioAnim = initObjAnim(rend, MARIO_TEXTURE, mTexBox);
//	printf("Texture OK\n");
	SDL_Rect marioBox;
	marioBox.x = x;
	marioBox.y = y;
	marioBox.h = BLOCK_SIZE;
	marioBox.w = BLOCK_SIZE;
	Obj *mario = initObject(marioBox, marioAnim, TYPE_PLAYER);
	mario->x_speed = 0;
	mario->objects_below = initObjList();
	mario->objects_right = initObjList();
	mario->objects_left = initObjList();
	mario->objects_over = initObjList();
//	printf("Player OK\n");
	return mario;
}
