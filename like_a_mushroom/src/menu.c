#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "menu.h"

MainMenu *initMainMenu(SDL_Renderer *rend)
{
	MainMenu *mmenu = malloc(sizeof(MainMenu));
	mmenu->font = TTF_OpenFont("textures/main_font.ttf", 20);
	mmenu->txt_color = malloc(sizeof(SDL_Color));
	mmenu->bg_color = malloc(sizeof(SDL_Color));
	mmenu->txt_color->r = 0;
	mmenu->txt_color->g = 0;
	mmenu->txt_color->b = 0;
	mmenu->txt_color->a = 0;
	mmenu->bg_color->a = 1;
	SDL_Surface *load_surf;
	for(int i = 1; i <= 5; i += 1)
	{
		if(i == 1)
		{
			load_surf = TTF_RenderText_Shaded(mmenu->font, "ИГРАТЬ", *mmenu->txt_color, *mmenu->bg_color);
			mmenu->pos1 = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		if(i == 2)
		{
			load_surf = TTF_RenderText_Shaded(mmenu->font, "НАСТРОЙКИ", *mmenu->txt_color, *mmenu->bg_color);
			mmenu->pos1 = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		if(i == 3)
		{
			load_surf = TTF_RenderText_Shaded(mmenu->font, "ВЫХОД", *mmenu->txt_color, *mmenu->bg_color);
			mmenu->pos1 = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		free(load_surf);
	}

	return mmenu;
}
