#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "menu.h"


MainMenu *initMainMenu(SDL_Renderer *rend)
{
	MainMenu *mmenu = malloc(sizeof(MainMenu));
	mmenu->font = TTF_OpenFont("textures/main_font.ttf", 40);
	mmenu->menu_bg = IMG_LoadTexture(rend, "textures/main_menu_bg.jpg");
	mmenu->select_rect = IMG_LoadTexture(rend, "textures/select_rect.jpg");
	mmenu->txt_color = malloc(sizeof(SDL_Color));
	mmenu->bg_color = malloc(sizeof(SDL_Color));
	mmenu->txt_color->r = 255;
	mmenu->txt_color->g = 255;
	mmenu->txt_color->b = 255;
	mmenu->txt_color->a = 1;
	mmenu->bg_color->r = 255;
	mmenu->bg_color->g = 255;
	mmenu->bg_color->b = 255;
	mmenu->bg_color->a = 0;
	SDL_Surface *load_surf;
	for(int i = 0; i <= 2; i ++)
	{
		if(i == 0)
		{
			load_surf = TTF_RenderText_Shaded(mmenu->font, "Play", *mmenu->txt_color, *mmenu->bg_color);
			mmenu->pos1 = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		if(i == 1)
		{
			load_surf = TTF_RenderText_Shaded(mmenu->font, "Options", *mmenu->txt_color, *mmenu->bg_color);
			mmenu->pos2 = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		if(i == 2)
		{
			load_surf = TTF_RenderText_Shaded(mmenu->font, "Exit", *mmenu->txt_color, *mmenu->bg_color);
			mmenu->pos3 = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		free(load_surf);
	}
	mmenu->current_pos = 0;
	return mmenu;
}


void addToRenderMainMenu(MainMenu *mmenu, SDL_Renderer *rend, int screen_width, int screen_height)
{
	SDL_Rect *text_box = calloc(1, sizeof(SDL_Rect));
	SDL_RenderCopy(rend, mmenu->menu_bg, NULL, NULL);
	int dy =  screen_height/8;
	SDL_Rect *select_box = malloc(sizeof(SDL_Rect));
	TTF_SizeText(mmenu->font, "--OPTIONS--", &select_box->w, &select_box->h);
	select_box->h += 10;
	select_box->x = screen_width/2 - select_box->w/2;
	select_box->y = dy*(mmenu->current_pos + 2) - select_box->h/2;
	SDL_RenderCopy(rend, mmenu->select_rect, NULL, select_box);
	for(int i = 0; i <= 2; i++)
	{
		if(i == 0)
		{
			TTF_SizeText(mmenu->font, "Play", &text_box->w, &text_box->h);
			text_box->y = dy*(i + 2) - text_box->h/2;
			text_box->x = screen_width/2 - text_box->w/2;
			SDL_RenderCopy(rend, mmenu->pos1, NULL, text_box);
		}
		if(i == 1)
		{
			TTF_SizeText(mmenu->font, "Options", &text_box->w, &text_box->h);
			text_box->y = dy*(i + 2) - text_box->h/2;
			text_box->x = screen_width/2 - text_box->w/2;
			SDL_RenderCopy(rend, mmenu->pos2, NULL, text_box);
		}
		if(i == 2)
		{
			TTF_SizeText(mmenu->font, "Exit", &text_box->w, &text_box->h);
			text_box->y = dy*(i + 2) - text_box->h/2;
			text_box->x = screen_width/2 - text_box->w/2;
			SDL_RenderCopy(rend, mmenu->pos3, NULL, text_box);
		}
	}
	free(text_box);
}
