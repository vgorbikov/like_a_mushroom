#ifndef map_funcs
#define map_funcs


#define RENDERING_AREA_HEIGHT 28
#define RENDERING_AREA_WIDTH 47
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


ObjList *mapLoad(SDL_Renderer *rend);


Obj *getPlayer(ObjList *map);


int mapRender(ObjList *map, SDL_Renderer *rend);


void moveMap(ObjList *map, int dx);

void trackThePlayer(ObjList *map, Obj *player);

#endif
