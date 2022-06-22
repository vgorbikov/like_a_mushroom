#ifndef status_bar
#define status_bar

typedef struct StatusBar
{
	int score, coins, lives;
	long int time;
	int world, room;
} StatusBar;


StatusBar *initStatusBar(int world, int room, int time);


void addSBarToRender(StatusBar *bar, SDL_Renderer *rend, int screen_w, int screen_h);


#endif
