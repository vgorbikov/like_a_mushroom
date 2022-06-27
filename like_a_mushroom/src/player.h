#ifndef player_person
#define player_person

#define PLAYER_RUN_SPEED 500
#define PLAYER_RUN_ANIMATION_DURATION 500
#define PLAYER_JUMP_SPEED 700

Obj *initPlayer(SDL_Renderer *rend, int x, int y);

void updatePlayerRunAnim(Obj *obj, int direction);

void updatePlayerStaticAnim(Obj *obj);

void updatePlayerJumpAnim(Obj *obj);

void updatePlayerDeathAnim(Obj *obj);

void playerDeath(Obj *player);

int playerTouch(Obj *player, Obj* obj, int touch_code);

int playerEventHandler(Obj *player);

#endif
