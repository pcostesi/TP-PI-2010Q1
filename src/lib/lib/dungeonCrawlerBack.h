#define POTCHANCE 25
#define POTRESTORE 0.5

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
