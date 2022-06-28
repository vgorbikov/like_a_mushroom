#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "status_bar.h"



StatusBar *initStatusBar(int world, int room, int time, int lives, int score,  SDL_Renderer *rend)
{
	StatusBar *bar = malloc(sizeof(StatusBar));
	bar->coins = 0;
	bar->score = score;
	bar->world = world;
	bar->room = room;
	bar->lives = lives;
	bar->time = time;
	bar->font = TTF_OpenFont("textures/main_font.ttf", 20);
	SDL_Surface *load_surf;
	bar->txt_color = malloc(sizeof(SDL_Color));
	bar->bg_color = malloc(sizeof(SDL_Color));
	bar->txt_color->r = 0;
	bar->txt_color->g = 0;
	bar->txt_color->b = 0;
	bar->txt_color->a = 0;
	bar->bg_color->a = 1;
	char buf[10];
	for(int i = 1; i <= 5; i += 1)
	{
		if(i == 1)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "SCORE", *bar->txt_color, *bar->bg_color);
			bar->score_tex = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
			sprintf(buf, "%i", bar->score);
			load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
			bar->score_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		if(i == 2)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "COINS", *bar->txt_color, *bar->bg_color);
			bar->coins_tex = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
			sprintf(buf, "%i", bar->coins);
			load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
			bar->coins_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		if(i == 3)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "WORLD", *bar->txt_color, *bar->bg_color);
			bar->world_tex = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
			sprintf(buf, "%i-%i", bar->world, bar->room);
			load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
			bar->world_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		if(i == 4)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "TIME", *bar->txt_color, *bar->bg_color);
			bar->time_tex = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
			sprintf(buf, "%li", bar->time);
			load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
			bar->time_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		if(i == 5)
		{
			load_surf = TTF_RenderText_Shaded(bar->font, "LIVES", *bar->txt_color, *bar->bg_color);
			bar->lives_tex = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
			sprintf(buf, "%i", bar->lives);
			load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
			bar->lives_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
		}
		free(load_surf);
	}
	return bar;
}


void updateBarTimeTex(StatusBar *bar, SDL_Renderer *rend)
{
	char buf[10];
	sprintf(buf, "%li", bar->time);
	SDL_Surface *load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
	SDL_DestroyTexture(bar->time_counter_tex);
	bar->time_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
	SDL_FreeSurface(load_surf);
}



void updateBarScoreTex(StatusBar *bar, SDL_Renderer *rend)
{
	char buf[10];
	sprintf(buf, "%i", bar->score);
	SDL_Surface *load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
	SDL_DestroyTexture(bar->score_counter_tex);
	bar->score_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
	SDL_FreeSurface(load_surf);
}



void updateBarCoinsTex(StatusBar *bar, SDL_Renderer *rend)
{
	char buf[10];
	sprintf(buf, "%i", bar->coins);
	SDL_Surface *load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
	SDL_DestroyTexture(bar->coins_counter_tex);
	bar->coins_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
	SDL_FreeSurface(load_surf);
}



void updateBarLivesTex(StatusBar *bar, SDL_Renderer *rend)
{
	char buf[10];
	sprintf(buf, "%i", bar->lives);
	SDL_Surface *load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
	SDL_DestroyTexture(bar->lives_counter_tex);
	bar->lives_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
	SDL_FreeSurface(load_surf);
}


void updateBarWorldTex(StatusBar *bar, SDL_Renderer *rend)
{
	char buf[10];
	sprintf(buf, "%i-%i", bar->world, bar->room);
	SDL_Surface *load_surf = TTF_RenderText_Shaded(bar->font, buf, *bar->txt_color, *bar->bg_color);
	SDL_DestroyTexture(bar->coins_counter_tex);
	bar->coins_counter_tex = SDL_CreateTextureFromSurface(rend, load_surf);
	SDL_FreeSurface(load_surf);
}


void addSBarToRender(StatusBar *bar, SDL_Renderer *rend, int screen_w, int screen_h)
{
	TTF_Font *font = bar->font;

	int dx = screen_w/6;
	SDL_Rect *text_box = calloc(1, sizeof(SDL_Rect));
	char buf[10];
	for(int i = 1; i <= 5; i += 1)
	{

		text_box->y = 10;

		if(i == 1)
		{
			TTF_SizeText(font, "SCORE", &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->score_tex, NULL, text_box);
			text_box->y += text_box->h + 5;
			sprintf(buf, "%i", bar->score);
			TTF_SizeText(font, buf, &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->score_counter_tex, NULL, text_box);
		}
		if(i == 2)
		{
			TTF_SizeText(font, "COINS", &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->coins_tex, NULL, text_box);
			text_box->y += text_box->h + 5;
			sprintf(buf, "%i", bar->coins);
			TTF_SizeText(font, buf, &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->coins_counter_tex, NULL, text_box);
		}
		if(i == 3)
		{
			TTF_SizeText(font, "WORLD", &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->world_tex, NULL, text_box);
			text_box->y += text_box->h + 5;
			sprintf(buf, "%i-%i", bar->world, bar->room);
			TTF_SizeText(font, buf, &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->world_counter_tex, NULL, text_box);
		}
		if(i == 4)
		{
			TTF_SizeText(font, "TIME", &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->time_tex, NULL, text_box);
			text_box->y += text_box->h + 5;
			sprintf(buf, "%li", bar->time);
			TTF_SizeText(font, buf, &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->time_counter_tex, NULL, text_box);
		}
		if(i == 5)
		{
			TTF_SizeText(font, "LIVES", &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->lives_tex, NULL, text_box);
			text_box->y += text_box->h + 5;
			sprintf(buf, "%i", bar->lives);
			TTF_SizeText(font, buf, &text_box->w, &text_box->h);
			text_box->x = dx*i - text_box->w/2;
			SDL_RenderCopy(rend, bar->lives_counter_tex, NULL, text_box);
		}
	}
	free(text_box);
}
