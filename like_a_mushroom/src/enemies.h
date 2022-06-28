#ifndef enemies
#define enemies


#define MARIO_RUN_SPEED 100
#define MARIO_RUN_ANIMATION_DURATION 600
#define MARIO_JUMP_SPEED 400


Obj *initMario(SDL_Renderer *rend, int x, int y);


void updateMarioRunAnim(Obj *obj, int direction);


void updateMarioStaticAnim(Obj *obj);


void updateMarioJumpAnim(Obj *obj);

void updateMarioDeathAnim(Obj *obj);

void marioDeath(Obj *mario);

int marioEventHandler(Obj *mario);

void enemiesControl(ObjList *ene);

#endif //enemies
