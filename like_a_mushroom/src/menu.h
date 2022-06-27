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
	SDL_Texture *menu_bg;
	SDL_Texture *select_rect;
} MainMenu;


MainMenu *initMainMenu(SDL_Renderer *rend);

void addToRenderMainMenu(MainMenu *mmenu, SDL_Renderer *rend, int screen_width, int screen_height);

#endif
