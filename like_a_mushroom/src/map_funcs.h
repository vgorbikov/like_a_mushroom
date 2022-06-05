#ifndef map_funcs
#define map_funcs


#define RENDERING_AREA_HEIGHT 28
#define RENDERING_AREA_WIDTH 47
#define BLOCK_SIZE 18


typedef struct Map
{
	int height, width;
	ObjList *tiles;
} Map;


void addObjInMap(Map *map, Obj *obj);


void delObjFromMap(Map *map, Obj *obj);


void moveObj(Map *map, Obj *obj, int dx, int dy);


Map *mapLoad(SDL_Renderer *rend);

#endif
