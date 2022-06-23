#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "status_bar.h"



StatusBar *initStatusBar(int world, int room, int time, SDL_Renderer *rend)
{
	StatusBar *bar = malloc(sizeof(StatusBar));
	bar->coins = 0;
	bar->score = 0;
	bar->world = world;
	bar->room = room;
	bar->lives = 3;
	bar->time = time;
	bar->font = TTF_OpenFont("textures/main_font.ttf", 20);

	SDL_Surface *load_surf;
	SDL_Color text_color;
	text_color.r = 0;
	text_color.g = 0;
	text_color.b = 0;
	text_color.a = 0;
	SDL_Color bg_color;
	bg_color.a = 1;
	for(int i = 1; i <= 5; i += 1)
	{
		if(i == 1)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "SCORE", text_color, bg_color);
			bar->score_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		if(i == 2)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "COINS", text_color, bg_color);
			bar->coins_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		if(i == 3)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "WORLD", text_color, bg_color);
			bar->world_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		if(i == 4)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "TIME", text_color, bg_color);
			bar->time_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		if(i == 5)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "LIVES", text_color, bg_color);
			bar->lives_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		free(load_surf);
	}
	return bar;
}




void addSBarToRender(StatusBar *bar, SDL_Renderer *rend, int screen_w, int screen_h)
{
	TTF_Font *font = bar->font;
//	SDL_Color text_color;
//	text_color.r = 0;
//	text_color.g = 0;
//	text_color.b = 0;
//	text_color.a = 0;
//	SDL_Color bg_color;
//	bg_color.a = 1;

	int dx = screen_w/6;
	SDL_Rect *textBox = calloc(1, sizeof(SDL_Rect));

	for(int i = 1; i <= 5; i += 1)
	{

		textBox->y = 10;

		if(i == 1)
		{
			TTF_SizeText(font, "SCORE", &textBox->w, &textBox->h);
			textBox->x = dx*i - textBox->w/2;
			SDL_RenderCopy(rend, bar->score_tex, NULL, textBox);
		}
		if(i == 2)
		{
			TTF_SizeText(font, "COINS", &textBox->w, &textBox->h);
			textBox->x = dx*i - textBox->w/2;
			SDL_RenderCopy(rend, bar->coins_tex, NULL, textBox);
		}
		if(i == 3)
		{
			TTF_SizeText(font, "WORLD", &textBox->w, &textBox->h);
			textBox->x = dx*i - textBox->w/2;
			SDL_RenderCopy(rend, bar->world_tex, NULL, textBox);
		}
		if(i == 4)
		{
			TTF_SizeText(font, "TIME", &textBox->w, &textBox->h);
			textBox->x = dx*i - textBox->w/2;
			SDL_RenderCopy(rend, bar->time_tex, NULL, textBox);
		}
		if(i == 5)
		{
			TTF_SizeText(font, "LIVES", &textBox->w, &textBox->h);
			textBox->x = dx*i - textBox->w/2;
			SDL_RenderCopy(rend, bar->lives_tex, NULL, textBox);
		}
	}
	free(textBox);
}
