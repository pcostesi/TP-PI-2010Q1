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
}




/*Simulates the combat between the player and any posible enemy*/
int
combat(character_t *player, enemy_t *enemy, profession_t *profession, game_t * actualGame)
{
    int winner, turn, hit, enemyHP;
    float starter = ((float)(rand())/(RAND_MAX));
    starter < 0.5 ? (turn = 0) : (turn = 1);
    enemyHP = (enemy->minHP + ( (float)(rand())/RAND_MAX ) * (enemy->maxHP - enemy->minHP));
    printf("In the room you encounter a %s, who isn't willing to let you pass, he has %d HP \n", (enemy->name), enemyHP);
    printf("You engage a %s in battle: \n\n", enemy->name);


    while (player->HP > 0 && enemyHP > 0)
    {
            if(turn % 2 == 0)
            {
                hit = damageRoll(profession->maxDP, profession->minDP);
                enemyHP -= hit;
                printf("%s has been hit for %d, leaving him with %d health point remaining.\n\n Please press enter to continue.\n\n\n", enemy->name, hit, enemyHP);
                PAUSE;
                turn++;
            }
            else
            {
                hit = damageRoll(enemy->maxDP[getProfessionIndexByID(actualGame, player->professionID)], enemy->minDP[getProfessionIndexByID(actualGame, player->professionID)]);
                player->HP -= hit;
                printf("%s has been hit for %d, leaving him with %d health point remaining\n\n Please press enter to continue.\n\n\n", player->name, hit, player->HP);
                PAUSE;
                turn++;
            }
    }
    if (player->HP <= 0)
    {
        printf("You have been defeated, and thus have dishonored your family \n");
                return DEAD;
    }
    else
    {
        printf("You have crushed another foolish enemy thata got in yur way \n\n");
                return ALIVE;
    }
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
