#define DELETE_BUFFER while(getchar() != '\n')
#define ALIVE 1
#define DEAD 0
#define PAUSE while(getchar() != '\n')
#define INVALID 0
#define VALID 1
#define WON 2
#define MAX_INPUT 32
#define NAME_TOO_SHORT 4
#define NAME_TOO_LONG 2
#define BAD_NAME_FORMAT 3

#include "libparse.h"
#include "strings.h"
#include "libcrawl.h"
#include "state.h"
#include "screen.h"
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

int damageRoll(int max, int min);
int drinkPotion(character_t *player);
int getPotion(character_t *player);
