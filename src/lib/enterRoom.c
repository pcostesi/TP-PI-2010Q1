#define DELETE_BUFFER while(getchar() != '\n')
#define ALIVE 1
#define DEAD 0
#define PAUSE while(getchar() != '\n')
#define INVALID 0
#define VALID 1
#define WON 2

#include "libparse.h"
#include "libcrawl.h"
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
int enterRoom(character_t *player, room_t *actualRoom, game_t *actualGame);

int
main(int argcount, char ** vector_of_strings)
{
    char fileName[36];
    int status = ALIVE, i, aux;
    printf("Insert the name of the source file, without the extension \n");
    getName(fileName);
    strcat(fileName, ".xml");
    game_t *actualGame = load_game(fileName);
    srand(time(NULL));

    character_t *player;
    player = malloc(sizeof(character_t));
    if (player == NULL)
    {
         printf("Fatal Error \n");
         return 0;
    }


    player->name = malloc(sizeof(char*)*32);
    if (player->name == NULL)
         {
         printf("Fatal Error \n");
         return 0;
    }

    printf("Inster the name of the avatar \n");
    getName(player->name);

    chooseProfession(player, actualGame );
    room_t *actualRoom = getRoomByID(actualGame, actualGame->StartRoomID);
    status = enterRoom( player, actualRoom, actualGame);
    while( status == ALIVE )
    {
        printf("In the actual room you see %d doors, which one do u wish to take? \n", actualRoom->gates_size );
                do
                {
                        for(i = 0; i < actualRoom->gates_size; i++)
                        {
                                printf("You may choose door number %d, wich is the %s \n", (i+1), actualRoom->gates[i]->name);
                        }
                        aux = getint("Select the desired door \n\n\n");
                } while( aux > i || aux < 0 );
                status = enterRoom(player, getRoomByID(actualGame, actualRoom->gates[aux-1]->room_id), actualGame);
                actualRoom = getRoomByID(actualGame, actualRoom->gates[aux-1]->room_id);
    }
        if(status == DEAD);
                printf("You have been defeated \n");
        if(status == WON)
                printf("You have succesfully escaped the dungeon, congratulations!!\n");
        free(player->name);
        free(player);

    return EXIT_SUCCESS;
}

void
getName(char name[])
{
    int i, validName = INVALID, a;
    while( validName == INVALID)
    {
                validName = VALID;
                i = 0;
                char buff = getchar();
                while( buff != '\n' && i < 30 )
                {
                        name[i] = buff;
                        i++;
                        buff = getchar();
                }

                name[i] = '\0';

                if( i == 0)
                {
                        printf("Insert at least 1 character\n");
                        DELETE_BUFFER;
                        validName = INVALID;
                }
                else if( i == 30)
                {
                        printf("The name is too long, insert a valid name with less than 31 characters please \n");
                        DELETE_BUFFER;
                        validName = INVALID;
                }
                else if( name[0] == ' ' || name[i-1] == ' ')
                {
                        printf("The name must start and end with a number or digit \n");
                        DELETE_BUFFER;
                        validName = INVALID;
                }
                else
                {
                        for(a = 0; a < i && validName ; a++)
                        {
                                if(!(isalpha(name[a]) || name[a] == ' ' || isdigit(name[a])) )
                                        validName = INVALID;
                        }
                        if(!validName)
                                printf("Incorrect name, inster only characters, numbers or spaces, please \n");
                        if (validName)
                          return;
                }
        }
}


void
chooseProfession(character_t *player, game_t *actualGame)
{
        int i = 0, aux;
        printf("The available professions are:\n");
        while( i < actualGame->professions_size )
        {
                printf("%d - %s\n", i+1, actualGame->professions[i]->name);
                i++;
        }
        do
        {
                aux = getint("\n Select your desired profession \n");
        }while( aux > i || aux < 0 );
        player->professionID = actualGame->professions[aux-1]->ID;
        player->HP = actualGame->professions[aux-1]->minHP + ( (float)(rand())/RAND_MAX ) *
          (actualGame->professions[aux-1]->maxHP - actualGame->professions[aux-1]->minHP);
        printf("%s, you have choosen to be a %s, your initial health points are %d\n",player->name, actualGame->professions[aux-1]->name, player->HP);
        return;
}


/*Checks if the player has been into the actual room, if he hasn't it generates the fiends that  there will be in there, and simulate the combat
returnig if player is dead or alive*/
int
enterRoom(character_t *player, room_t *actualRoom, game_t *actualGame)
{
    int status = ALIVE, i = 0;
    player->roomID = actualRoom->ID;
    printf(" %s, you are standing in the room: %s \n\n", player->name, actualRoom->name);
    roomDescription(*actualRoom);
    if (actualRoom->visited == 0)
    {
        while( status == ALIVE && i < actualRoom->enemies_size)
        {
            status = combat(player, getEnemyByID(actualGame, actualRoom->enemy_ids[i]), getProfessionByID(actualGame, player->professionID), actualGame);
            i++;
        }
        actualRoom->visited = 1;
    }
        if(actualRoom->ID == actualGame->ExitRoomID && status != DEAD)
                status = WON;
    return status;
}




/*prints the description of the room*/
void
roomDescription(room_t actualRoom)
{
    int i = 0;
    char buff = actualRoom.description[0];
    while( buff != '\0' )
    {
        putchar(buff);
        i++;
        buff = actualRoom.description[i];
    }
    printf("\n");
    return;
}


int
combat(character_t *player, enemy_t *enemy, profession_t *profession, game_t * actualGame)
{
    int winner, turn, hit, enemyHP;
    float starter = ((float)(rand())/(RAND_MAX));
    starter < 0.5 ? (turn = 0) : (turn = 1);
    enemyHP = (enemy->minHP + ( (float)(rand())/RAND_MAX ) * (enemy->maxHP - enemy->minHP));
    printf("In the room you encounter a %s, who isn't willing to let you pass, he has %d HP \n", (enemy->name), enemyHP);
    printf("You engage a %s in battle: \n", enemy->name);

    while (player->HP > 0 && enemyHP > 0)
    {
            if(turn % 2 == 0)
            {
                hit = damageRoll(profession->maxDP, profession->minDP);
                enemyHP -= hit;
                printf("%s has been hit for %d, leaving him with %d health point remaining.\n Please press enter to continue.\n", enemy->name, hit, enemyHP);
                PAUSE;
                turn++;
            }
            else
            {
                hit = damageRoll(enemy->maxDP[getProfessionIndexByID(actualGame, player->professionID)], enemy->minDP[getProfessionIndexByID(actualGame, player->professionID)]);
                player->HP -= hit;
                printf("%s has been hit for %d, leaving him with %d health point remaining\n Please press enter to continue.\n", player->name, hit, player->HP);
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
        printf("You have crushed another foolish enemy thata got in your way \n");
                return ALIVE;
    }
}






int
damageRoll(int max, int min)
{
    return (min + ( (float)(rand())/RAND_MAX ) * (max - min) );
}
