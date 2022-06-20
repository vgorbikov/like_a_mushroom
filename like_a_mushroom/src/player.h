#ifndef player_person
#define player_person

#define PLAYER_RUN_SPEED 500
#define PLAYER_RUN_ANIMATION_DURATION 500

Obj *initPlayer(SDL_Renderer *rend, int x, int y);

int playerRun(Obj *player, int direction);

int gravitation(Obj *obj);

void playerTouchMonolith(Obj *player, Obj* monolith, int touch_code);

int playerStop(Obj *player, int direction, long int stime);

void updatePlayerRunAnim(Obj *obj, int direction);

void updatePlayerStaticAnim(Obj *obj);

void updatePlayerJumpAnim(Obj *obj, int direction);

void belowCalculator(Obj *obj);

int playerEventHandler(Obj *player);

#endif
