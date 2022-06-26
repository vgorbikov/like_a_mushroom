#ifndef map_funcs
#define map_funcs


#define RENDERING_AREA_HEIGHT 28
#define RENDERING_AREA_WIDTH 47
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define MAP1_TIME 300


Map *initMap();

Map *mapLoad(SDL_Renderer *rend);


Obj *getPlayer(ObjList *map);


ObjList *getMovable(ObjList *map);


int mapRender(Map *map, SDL_Renderer *rend);


void moveMap(Map *map, int dx);


void trackThePlayer(Map *map, Obj *player);

#endif
