#include "dungeonCrawlerBack.h"


/*Receives 2 ints, and generates a randome number between them*/
int
damageRoll(int max, int min)
{
    return (min + ( (float)(rand())/RAND_MAX ) * (max - min) );
}


/*Restores 50% of the health the player is lacking*/
int
drinkPotion(character_t *player)
{
  int restored;
  if(player->potions > 0)
  {
    restored = (player->maxHP - player->HP) * 0.5 ;
    player->potions--;
    player->HP += restored;
    return restored;
  }
  return -1;
}


/*Determines if the player will find or not a healing potion*/
int
getPotion(character_t *player)
{
  int chance = (25 + ( (float)(rand())/RAND_MAX ) * 10 );
  if(chance > ((float)(rand())/RAND_MAX )*100)
  {
    player->potions++;
    return player->potions;
  }
  return 0;
}
