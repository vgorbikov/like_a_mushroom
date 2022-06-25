#ifndef player_person
#define player_person

#define PLAYER_RUN_SPEED 500
#define PLAYER_RUN_ANIMATION_DURATION 500
#define PLAYER_JUMP_SPEED 400

Obj *initPlayer(SDL_Renderer *rend, int x, int y);

void updatePlayerRunAnim(Obj *obj, int direction);

void updatePlayerStaticAnim(Obj *obj);

void updatePlayerJumpAnim(Obj *obj);

int playerEventHandler(Obj *player);

#endif
