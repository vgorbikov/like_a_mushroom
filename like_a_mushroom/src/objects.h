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


/**
 * Описывает параметры текущей анимации объекта
 */
typedef struct ObjAnimation
{
	int status;
	SDL_Texture *texture;
	SDL_Rect *tex_box;
	int angle;
	SDL_RendererFlip flip;
	long int start_moment;
}ObjAnim;



/**
 * Хранит всю информацию о состоянии объекта
 * Скорость указывается в пикселях в секунду
 */
typedef struct Object
{
	SDL_Rect box;
	int type;
	void *map;
	float x_speed, y_speed, x_boost, y_boost, weight;
	long int last_xmove, last_ymove;
	SDL_Point moving;
	ObjAnim *animation;
	int status;
} Obj;



/**
 * Элемент-"обёртка" для объектов в списке.
 * Нужен в основном для того чтобы разгрузить поля струтуры объекта от лишних
 * указателей и иметь возможность двум разным спискам содержать один и тот же объект
 */
typedef struct ObjListElem
{
	Obj *object;
	struct ObjListElem *next;
	struct ObjListElem *prev;
}OLE;



/**
 * Хранит указатели на голову и текущий элемент списка.
 * Все объекты в списке обёрнуты в структуру ObjListElement
 */
typedef struct Objlist
{
	OLE *head;
	OLE *current;
}ObjList;


ObjAnim *initObjAnim(SDL_Renderer *rend, char *path, SDL_Rect *rect);


Obj *initObject(SDL_Rect obj_box, ObjAnim *tex, float weight, int type);


OLE *initOLE(Obj *obj);


ObjList *initObjList();


int addObjInList(ObjList *list, Obj *obj);


void nextObjInList(ObjList *list);


void headObjInList(ObjList *list);


int delObjFromList(ObjList *list);


int delObjList(ObjList *list);


int objInList(ObjList *list, Obj *obj);

void moveObj(Obj *obj);

void setObjAnimation(Obj *obj, int animation_type);

int movingCalculator(Obj *obj);

ObjList *objectsNearby(Obj *obj);

SDL_Rect *touchingCalculator(Obj *obj1, Obj *obj2);

#endif
