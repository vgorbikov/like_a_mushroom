#ifndef objects
#define objects

#define BLOCK_SIZE 28

#define STONE_CODE 0
#define STONE_TEXTURE "textures/stone.png"
#define WOOD_CODE 102
#define WOOD_TEXTURE "textures/wood.png"
#define PLAYER_CODE 79
#define PLAYER_TEXTURE "textures/player_anim.png"

#define STATIC_STATUS 0
#define RUN_STATUS 10
#define JUMP_STATUS 20
#define HITED_STATUS 30

#define TYPE_MONOLITH 0
#define TYPE_PLAYER 10

#define LEFT_TOUCH 1
#define TOP_TOUCH 2
#define RIGHT_TOUCH 3
#define BOTTOM_TOUCH 4

#define RUN_RIGHT 1
#define RUN_LEFT 2

typedef struct Line
{
	SDL_Point begin;
	SDL_Point end;
}Line;


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


typedef struct ObjEvent
{
	long int start_moment;
	int event_code;
} ObjEvent;


typedef struct EventListElem
{
	ObjEvent *event;
	struct EventListElem *next;
	struct EventListElem *prev;
}ELE;


/**
 * Хранит указатели на голову и текущий элемент списка.
 * Все объекты в списке обёрнуты в структуру ObjListElement
 */
typedef struct Eventlist
{
	ELE *head;
	ELE *current;
}EventList;


/**
 * Хранит всю информацию о состоянии объекта
 * Скорость указывается в пикселях в секунду
 */
typedef struct Object
{
	SDL_Rect box;
	int type;
	float x_speed, y_speed, x_boost, y_boost;
	long int last_xmove, last_ymove; //Хранит время последнего перемещения по соответствующим осям
	SDL_Point moving; //Хранит вектор перемещания
	ObjAnim *animation;
	EventList *events;
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


ObjEvent *initEvent(int event_code);


void initLine(Line line, int x1, int y1, int x2, int y2);


Line *getMovingLines(SDL_Rect *rect, SDL_Point *moving);


ObjAnim *initObjAnim(SDL_Renderer *rend, char *path, SDL_Rect *rect);


Obj *initObject(SDL_Rect obj_box, ObjAnim *tex, int type);


OLE *initOLE(Obj *obj);


ObjList *initObjList();


int addObjInList(ObjList *list, Obj *obj);


void nextObjInList(ObjList *list);


void headObjInList(ObjList *list);


int delObjFromList(ObjList *list);


int delObjList(ObjList *list);


int objInList(ObjList *list, Obj *obj);


ELE *initELE(ObjEvent *obj);


EventList *initEventList();


int addEventInList(EventList *list, int event_code);


void nextEventInList(EventList *list);


void headEventInList(EventList *list);


int delEventFromList(EventList *list, int event_code);


int delEventList(EventList *list);


int EventInList(EventList *list, int event_code);


void setObjAnimation(Obj *obj, int animation_type);


int animationHandler(ObjList *list);


int eventHandler(ObjList *list);


int movingCalculator(Obj *obj);


//void moveObj(Obj *obj);


int touchingCalculator(Obj *obj1, Obj *obj2);

#endif
