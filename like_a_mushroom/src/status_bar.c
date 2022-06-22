#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "status_bar.h"



StatusBar *initStatusBar(int world, int room, int time)
{
	StatusBar *bar = malloc(sizeof(StatusBar));
	bar->coins = 0;
	bar->score = 0;
	bar->world = world;
	bar->room = room;
	bar->lives = 3;
	bar->time = time;
	return bar;
}

void addSBarToRender(StatusBar *bar, SDL_Renderer *rend, int screen_w, int screen_h)
{
	TTF_Font *font = TTF_OpenFont("textures/main_font.ttf", screen_h/20);
	SDL_Color text_color;
	text_color.r = 0;
	text_color.g = 0;
	text_color.b = 0;
	text_color.a = 0;
	SDL_Color bg_color;
	bg_color.a = 1;
//	SDL_Texture *score = SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "SCORE", text_color, bg_color));
//	SDL_Texture *lives = SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "LIVES", text_color, bg_color));
//	SDL_Texture *coins = SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "COINS", text_color, bg_color));
//	SDL_Texture *world = SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "WORLD", text_color, bg_color));
//	SDL_Texture *time = SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "TIME", text_color, bg_color));
	int dx = screen_w/6;
	for(int i = 1; i <= 5; i += 1)
	{
		SDL_Rect textBox;
		textBox.y = 10;
		if(i == 1)
		{
			TTF_SizeText(font, "SCORE", &textBox.w, &textBox.h);
			textBox.x = dx*i - textBox.w/2;
			SDL_RenderCopy(rend, SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "SCORE", text_color, bg_color)),
					NULL, &textBox);
		}
		if(i == 2)
		{
			TTF_SizeText(font, "COINS", &textBox.w, &textBox.h);
			textBox.x = dx*i - textBox.w/2;
			SDL_RenderCopy(rend, SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "COINS", text_color, bg_color)),
					NULL, &textBox);
		}
		if(i == 3)
		{
			TTF_SizeText(font, "WORLD", &textBox.w, &textBox.h);
			textBox.x = dx*i - textBox.w/2;
			SDL_RenderCopy(rend, SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "WORLD", text_color, bg_color)),
					NULL, &textBox);
		}
		if(i == 4)
		{
			TTF_SizeText(font, "TIME", &textBox.w, &textBox.h);
			textBox.x = dx*i - textBox.w/2;
			SDL_RenderCopy(rend, SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "TIME", text_color, bg_color)),
					NULL, &textBox);
		}
		if(i == 5)
		{
			TTF_SizeText(font, "LIVES", &textBox.w, &textBox.h);
			textBox.x = dx*i - textBox.w/2;
			SDL_RenderCopy(rend, SDL_CreateTextureFromSurface(rend, TTF_RenderText_Shaded(font, "LIVES", text_color, bg_color)),
					NULL, &textBox);
		}
	}


}
