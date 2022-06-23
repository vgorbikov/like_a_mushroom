#ifndef status_bar
#define status_bar

typedef struct StatusBar
{
	int score, coins, lives;
	long int time;
	int world, room;
	TTF_Font *font;
	SDL_Color txt_color, bg_color;
	SDL_Texture *score_tex;
	SDL_Texture *coins_tex;
	SDL_Texture *lives_tex;
	SDL_Texture *world_tex;
	SDL_Texture *time_tex;
	SDL_Texture *score_counter_tex;
	SDL_Texture *coins_counter_tex;
	SDL_Texture *lives_counter_tex;
	SDL_Texture *world_counter_tex;
	SDL_Texture *time_counter_tex;
} StatusBar;


StatusBar *initStatusBar(int world, int room, int time, SDL_Renderer *rend);

void updateBarTimeTex(StatusBar *bar, SDL_Renderer *rend);

void addSBarToRender(StatusBar *bar, SDL_Renderer *rend, int screen_w, int screen_h);


#endif
