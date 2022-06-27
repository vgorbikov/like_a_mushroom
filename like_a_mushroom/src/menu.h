#ifndef menu
#define menu

typedef struct MainMenu
{
	int current_pos;
	TTF_Font *font;
	SDL_Color *txt_color, *bg_color;
	SDL_Texture *pos1;
	SDL_Texture *pos2;
	SDL_Texture *pos3;
} MainMenu;

#endif
