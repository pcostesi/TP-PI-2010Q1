#include "dungeonCrawlerBack.h"



int
getName(char name[], int len)
{
    int i, validName = INVALID, a;
    while( validName == INVALID)
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
                        if(!validName)
                                return BAD_NAME_FORMAT;
                        if (validName)
                          return NAME_OK;
                }
        }
    return NAME_TOO_SHORT;
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
  if(player->potions == 0)
    return -1;
  else
  {
    restored = (player->maxHP - player->HP) * 0.5 ;
    player->potions--;
    player->HP += restored;
    return restored;
  }
  return;
}


/*Determines if the player will find or not a healing potion*/
int
getPotion(character_t *player)
{
  int chance = (25 + ( (float)(rand())/RAND_MAX ) * 10 );
  if(chance > ((float)(rand())/RAND_MAX )*100)
  {
    player->potions++;
    printf("The enemy has droped a healing potion that you could use\n\n");
  }
  return;
}
