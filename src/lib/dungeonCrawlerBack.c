#include "dungeonCrawlerBack.h"



int
getName(char name[], int len)
{
    int i, validName = VALID, a;
    i = 0;
    char buff;
    while((buff = getchar()) != '\n' && buff != EOF && validName != NAME_TOO_LONG)
    {
                validName = VALID;
                i = 0;
                char buff = getchar();
                while( buff != '\n' && i < len )
                {
                        name[i] = buff;
                        i++;
                        buff = getchar();
                }

                name[i] = '\0';

                if( i == 0)
                {
                        DELETE_BUFFER;
                        validName = INVALID;
                        return NAME_TOO_SHORT;
                }
                else if( i == len)
                {

                        DELETE_BUFFER;
                        validName = INVALID;
                        return NAME_TOO_LONG;
                }
                else if( name[0] == ' ' || name[i-1] == ' ')
                {
                        DELETE_BUFFER;
                        validName = INVALID;
                        return BAD_NAME_FORMAT;
                }
                else
                {
                        for(a = 0; a < i && validName ; a++)
                        {
                                if(!(isalpha(name[a]) || name[a] == ' ' || isdigit(name[a])) )
                                        validName = INVALID;
                        }
                }
        }
    return NAME_OK;
}


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