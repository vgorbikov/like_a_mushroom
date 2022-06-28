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
	mmenu->menu_bg = IMG_LoadTexture(rend, "textures/menu/main_menu_bg.png");
	mmenu->select_rect = IMG_LoadTexture(rend, "textures/menu/select_rect.jpg");
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
	free(select_box);
}


int updateConfigFile(ConfigParam *conf)
{
	FILE *conf_file = fopen("config.txt", "w");
	fprintf(conf_file, "%i\n", conf->screen_width);
	fprintf(conf_file, "%i\n", conf->screen_height);
	fprintf(conf_file, "%i\n", conf->control_type);
	fprintf(conf_file, "%i\n", conf->player_skin);
	fclose(conf_file);
	printf("UPDATE!\n");
	return 0;
}


ConfigParam *getConfigFromFile()
{
	FILE *conf_file = fopen("config.txt", "r");
	ConfigParam *conf = malloc(sizeof(ConfigParam));
	if(conf_file == NULL)
	{
		printf("No config\n");
		conf->screen_width = 1280;
		conf->screen_height = 720;
		conf->control_type = 0;
		conf->player_skin = 0;
	}
	else
	{
		if(feof(conf_file) != 0) conf->screen_width = 1280;
		else fscanf(conf_file, "%i", &conf->screen_width);
		if(feof(conf_file) != 0) conf->screen_height = 720;
		else fscanf(conf_file, "%i", &conf->screen_height);
		if(feof(conf_file) != 0) conf->control_type = 0;
		else fscanf(conf_file, "%i", &conf->control_type);
		if(feof(conf_file) != 0) conf->player_skin = 0;
		else fscanf(conf_file, "%i", &conf->player_skin);
		fclose(conf_file);
	}
	return conf;
}


OptionsMenu *initOptionsMenu(SDL_Renderer *rend, ConfigParam *conf)
{
	OptionsMenu *omenu = malloc(sizeof(OptionsMenu));
	omenu->font = TTF_OpenFont("textures/main_font.ttf", 40);
	omenu->menu_bg = IMG_LoadTexture(rend, "textures/menu/main_menu_bg.png");
	omenu->select_rect = IMG_LoadTexture(rend, "textures/menu/select_rect.jpg");
	omenu->txt_color = malloc(sizeof(SDL_Color));
	omenu->bg_color = malloc(sizeof(SDL_Color));
	omenu->txt_color->r = 255;
	omenu->txt_color->g = 255;
	omenu->txt_color->b = 255;
	omenu->txt_color->a = 1;
	omenu->bg_color->r = 255;
	omenu->bg_color->g = 255;
	omenu->bg_color->b = 255;
	omenu->bg_color->a = 0;
	SDL_Surface *load_surf;
	for(int i = 0; i <= 2; i ++)
	{
		if(i == 0)
		{
			load_surf = TTF_RenderText_Shaded(omenu->font, "Control", *omenu->txt_color, *omenu->bg_color);
			omenu->control_title = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		if(i == 1)
		{
			load_surf = TTF_RenderText_Shaded(omenu->font, "Skin", *omenu->txt_color, *omenu->bg_color);
			omenu->skin_title = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		if(i == 2)
		{
			load_surf = TTF_RenderText_Shaded(omenu->font, "Save&close", *omenu->txt_color, *omenu->bg_color);
			omenu->back_title = SDL_CreateTextureFromSurface(rend, load_surf);
			free(load_surf);
		}
		free(load_surf);
	}
	omenu->current_pos = 0;
	omenu->skin = conf->player_skin;
	omenu->control = conf->control_type;
	return omenu;
}




void addToRenderOptionsMenu(OptionsMenu *omenu, SDL_Renderer *rend, int screen_width, int screen_height)
{
	SDL_Rect *text_box = calloc(1, sizeof(SDL_Rect));
	SDL_RenderCopy(rend, omenu->menu_bg, NULL, NULL);
	int dy =  screen_height/8;
	SDL_Rect *select_box = malloc(sizeof(SDL_Rect));
	select_box->h = 80;
	select_box->w = screen_width/2;
	select_box->x = screen_width/2 - select_box->w/2;
	select_box->y = dy*(omenu->current_pos + 2) - select_box->h/2;
	SDL_RenderCopy(rend, omenu->select_rect, NULL, select_box);
	for(int i = 0; i <= 2; i++)
	{
		if(i == 0)
		{
			TTF_SizeText(omenu->font, "Skin: ", &text_box->w, &text_box->h);
			text_box->y = dy*(i + 2) - text_box->h/2;
			text_box->x = screen_width/2 - (text_box->w + 10);
			SDL_RenderCopy(rend, omenu->skin_title, NULL, text_box);
			char buf[100];
			sprintf(buf, "textures/player/player_%i.png", omenu->skin);
			SDL_Texture *skin = IMG_LoadTexture(rend, buf);
			SDL_Rect *skin_box = malloc(sizeof(SDL_Rect));
			skin_box->h = text_box->h;
			skin_box->w = text_box->h;
			skin_box->x = screen_width/2 + skin_box->w + 10;
			skin_box->y = text_box->y;
			SDL_Rect *box = malloc(sizeof(SDL_Rect));
			box->h = 100;
			box->w = 100;
			box->x = 0;
			box->y = 0;
			SDL_RenderCopy(rend, skin, box, skin_box);
			SDL_DestroyTexture(skin);
			free(skin_box);
			free(box);
		}
		if(i == 1)
		{
			TTF_SizeText(omenu->font, "Control: ", &text_box->w, &text_box->h);
			text_box->y = dy*(i + 2) - text_box->h/2;
			text_box->x = screen_width/2 - (text_box->w + 10);
			SDL_RenderCopy(rend, omenu->control_title, NULL, text_box);
			SDL_Texture *control_tex = IMG_LoadTexture(rend, "textures/menu/control_keys.png");
			SDL_Rect *control_box = malloc(sizeof(SDL_Rect));
			control_box->h = text_box->h;
			control_box->w = text_box->h + text_box->h/2;
			control_box->x = screen_width/2 + control_box->w + 10;
			control_box->y = text_box->y;
			SDL_Rect *box = malloc(sizeof(SDL_Rect));
			box->h = 100;
			box->w = 155;
			box->x = omenu->control*155;
			box->y = 0;
			SDL_RenderCopy(rend, control_tex, box, control_box);
			SDL_DestroyTexture(control_tex);
			free(control_box);
			free(box);
		}
		if(i == 2)
		{
			TTF_SizeText(omenu->font, "Save&close", &text_box->w, &text_box->h);
			text_box->y = dy*(i + 2) - text_box->h/2;
			text_box->x = screen_width/2 - text_box->w/2;
			SDL_RenderCopy(rend, omenu->back_title, NULL, text_box);
		}
	}
	free(text_box);
	free(select_box);
}
