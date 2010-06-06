#define DELETE_BUFFER while(getchar() != '\n')
#define ALIVE 1
#define DEAD 0
#define PAUSE while(getchar() != '\n')
#define INVALID 0
#define VALID 1
#define WON 2

#include "libparse.h"
#include "strings.h"
#include "libcrawl.h"
#include "state.h"
#include "getnum.h"
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

void chooseProfession(character_t *player, game_t *actualGame);
void getName(char name[]);
int combat(character_t *player, enemy_t *enemy, profession_t *profession, game_t * game);
void roomDescription(room_t actualRoom);
int damageRoll(int max, int min);
int enterRoom(character_t *player, room_t *actualRoom, game_t *actualGame, logbook log);
void drinkPotion(character_t *player);
void getPotion(character_t *player);
