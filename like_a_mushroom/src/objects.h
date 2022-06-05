#ifndef objects
#define objects

#define STONE_CODE 0
#define STONE_TEXTURE "textures/stone.png"
#define WOOD_CODE 102
#define WOOD_TEXTURE "textures/wood.png"

#define STATIC_STATUS 0
#define RUN_STATUS 10
#define JUMP_STATUS 20
#define HITED_STATUS 30

#define TYPE_MONOLITH 0
#define TYPE_PLAYER 10

typedef struct ObjTexture
{
	SDL_Texture *texture;
	SDL_Rect *tex_box;
}ObjTex;


typedef struct Object
{
	SDL_Rect box;
	void *map;
	float x_speed, y_speed, x_boost, y_boost, weight;
	int type;
	ObjTex *texture;
	int status;
	long int last_time;
} Obj;


typedef struct ObjListElem
{
	Obj *object;
	struct ObjListElem *next;
	struct ObjListElem *prev;
}OLE;


typedef struct Objlist
{
	OLE *head;
	OLE *current;
}ObjList;


ObjTex *initObjTex(SDL_Renderer *rend, char *path, SDL_Rect *rect);


Obj *initObject(SDL_Rect obj_box, ObjTex *tex, float weight, SDL_bool monolith);


OLE *initOLE(Obj *obj);


ObjList *initObjList();


int addObjInList(ObjList *list, Obj *obj);


void nextObjInList(ObjList *list);


void headObjInList(ObjList *list);


int delObjFromList(ObjList *list);


int delObjList(ObjList *list);


int objInList(ObjList *list, Obj *obj);

void moveObj(Obj *obj, int dx, int dy);


#endif
