#ifndef player_person
#define player_person


#define PLAYER_ANIMATION_DURATION 400

Obj *initPlayer(SDL_Renderer *rend, int x, int y);

void run(Obj *obj, int speed);

void updateRunAnim(Obj *obj, int direction);

void updateStaticAnim(Obj *obj);

#endif
