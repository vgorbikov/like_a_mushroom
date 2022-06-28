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


typedef struct OptionsMenu
{
	TTF_Font *font;
	SDL_Color *txt_color, *bg_color;
	SDL_Texture *menu_bg;
	SDL_Texture *select_rect;
	int current_pos;
	SDL_Texture *control_title;
	int control;
	SDL_Texture *skin_title;
	int skin;
	SDL_Texture *back_title;

} OptionsMenu;



typedef struct ConfigParam
{
	int screen_width, screen_height;
	int player_skin;
	int control_type;
} ConfigParam;


MainMenu *initMainMenu(SDL_Renderer *rend);

void addToRenderMainMenu(MainMenu *mmenu, SDL_Renderer *rend, int screen_width, int screen_height);

OptionsMenu *initOptionsMenu(SDL_Renderer *rend, ConfigParam *conf);

int updateConfigFile(ConfigParam *conf);

ConfigParam *getConfigFromFile();

void addToRenderOptionsMenu(OptionsMenu *omenu, SDL_Renderer *rend, int screen_width, int screen_height);

#endif
