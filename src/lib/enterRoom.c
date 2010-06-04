#define DELETE_BUFFER while(getchar() != '\n')
#define ALIVE 1
#define DEAD 0
#define PAUSE while(getchar() != '\n')
#define INVALID 0
#define VALID 1
#define WON 2

#define TORCH_HEIGHT 13
#define TORCH_MAX_WIDTH 8 

#define FATAL_ERROR_ROUTINE     clearDisplay(d, height, width);\
      errorStatus=changeDisplay(d,height,width,\
      IMG,1,0,0,TORCH,NULL, \
      IMG,1,0,72,TORCH,NULL, \
      printDisplay(d,uMargin,lMargin,height,width);\
      TEXT,2,5,10,"Fatal Error",NULL,\
      NULL);   \
      freeMatrix(d,height)\
      free(player->name);\
      free(player);\
      free_game(actualGame);\
      return 0

#include "libparse.h"
#include "libcrawl.h"
#include "getnum.h"
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
int menu(character_t *player, game_t *actualGame, displayType * d );
void chooseProfession(character_t *player, game_t *actualGame);
void getName(char name[]);
int combat(character_t *player, enemy_t *enemy, profession_t *profession, game_t * game);
void roomDescription(room_t actualRoom);
int damageRoll(int max, int min);
int enterRoom(character_t *player, room_t *actualRoom, game_t *actualGame);
void findPotion(player_t *player);
int playGame(character_t *player, game_t *actualGame, displayType * d);

int
main(int argcount, char ** vector_of_strings)
{
    char fileName[36];
    int status = ALIVE, i, aux;
    displayType * d;
    game_t *actualGame;
    character_t *player;
    room_t *actualRoom;
    player->potions = 0;  
    displayType * d;
    if(initializeDisplay(d,20,80,0,0,MIN_LAYER))
    {
      ERROR_MSG_MEM;
      return 0;
    }
    
    changeDisplay(d,    
    IMG,1,0,0,TORCH,NULL,	
    IMG,1,0,72,TORCH,NULL,
    NULL);
      
    getFileName(fileName, actualGame, d);
   
    if(argcount == 1)
	srand((*vector_of_strings)[0]);
    else
	srand(time(NULL));
  
    player = malloc(sizeof(character_t));
    if (player == NULL)
      FATAL_ERROR_ROUTINE;

    player->name = malloc(sizeof(char*)*36);
    if (player->name == NULL)
      FATAL_ERROR_ROUTINE;
    
    getName(player->name,d);
    
    chooseProfession(player, actualGame, d);
    actualRoom = getRoomByID(actualGame, actualGame->StartRoomID);
    status = enterRoom( player, actualRoom, actualGame, d);
    
    while( status == ALIVE )
      status=playGame(player,actualGame,d);
	
    if(status == DEAD);
    {
      errorStatus=changeDisplay(d,height,width,
      IMG,1,5,0,DEFEAT_LOGO,NULL,
      NULL);  
    }
    if(status == WON)
    {
      errorStatus=changeDisplay(d,height,width,
      IMG,1,5,0,VICTORY_LOGO,NULL,
      NULL);  
    }
    
    if(errorStatus!=OK)
      FATAL_ERROR_ROUTINE;

    printDisplay(d);
    freeDisplay(d);
    free(player->name);
    free(player);
    free_game(actualGame);
    
    return 0;
}

int
playGame(character_t *player, game_t *actualGame, displayType * d)
{
  int status,i;
  status = menu(player, actualGame, d );
  if(status == ALIVE)
  {
    for(i = 0; i < actualRoom->gates_size; i++)
    {
     changeDisplay(d,
     TEXT,3,TORCH_HEIGHT+3+i,10,sprintf("You may choose door number %d, wich is the %s \n", (i+1), actualRoom->gates[i]->name),NULL,
     TEXT,3,TORCH_HEIGHT+1,TORCH_MAX_WIDTH+2,"Please select a door",NULL,
     NULL); 
     printDisplay(d,uMargin,lMargin,height,width);
     clearArea(d,TORCH_HEIGHT+2,TORCH_HEIGHT+3,TORCH_MAX_WIDTH+2,(d->width)-TORCH_MAX_WIDTH-2)
    
     do
     {
       aux = getint("");	 
     }
     while(aux > i || aux < 1);
    }
  }
	
  clearArea(d,TORCH_HEIGHT+3,(d->height)-i-1,0,(d->width));
	
  status = enterRoom(player, getRoomByID(actualGame, actualRoom->gates[aux-1]->room_id), actualGame);
  actualRoom = getRoomByID(actualGame, actualRoom->gates[aux-1]->room_id);
  return status;
}

int
doorMenu(displayType * d)
{
	    changeDisplay(d,    
	    TEXT,2,5,10,strcat("In the actual room you see ",sprintf("%d doors,",actualRoom->gates_size)),NULL,
	    TEXT,2,6,12,"which do you choose?"),NULL,
	    TABLE,2,TORCH_HEIGHT+1,10,height-rows-2,width,NULL,
	    TEXT,3,TORCH_HEIGHT,10,"DOORS:",NULL,
	    NULL);  
	    printDisplay(d);
	    clearArea(d,0,TORCH_HEIGHT+1,MAX_TORCH_WIDTH,);
}


void
menu(character_t *player, game_t *actualGame, displayType * d )
{
  int aux = 0;
  do
  {
    changeDisplay(d,
    IMG,2,5,10,MENU_LOGO,NULL,
    STRETCH_BUTTON(2,6,10,3,"1.-Change Room"),NULL,
    STRETCH_BUTTON(3,9,10,3,"2.-Drink a health potion"),NULL,
    STRETCH_BUTTON(4,12,10,3,"3.-Save Game"),NULL,
    STRETCH_BUTTON(5,15,10,3,"4.-Load Game"),NULL,
    STRETCH_BUTTON(6,18,10,3,"5.-Quit Game"),NULL,
    NULL);
    printDisplay(d);
    clearArea(d,0,TORCH_HEIGHT+1,MAX_TORCH_WIDTH,);
    do
    {
      aux = getnum("");      
    }while( aux > 5 || aux < 1);
    
    switch(aux)
    {
      case 1: return ALIVE;
      case 2: drinkPotion(player);
	      break;
      case 3: save_state(actualGame, log, state.xml);
	      break;
      case 4: actualGame = load_state( log )
	      break;
      case 5: return DEAD;
    }
  } while();
    
}


void
getFileName(char * fileName, game_t *actualGame, displayType * d)
{
    do
    {
      changeDisplay(d,    
      TEXT,2,5,10,"Insert the name of the source file, without the extension:",NULL, 
      NULL);
      printDisplay(d);
      clearArea(d,5,5,10,strlen("Insert the name of the source file, without the extension:"));
    
      getName(fileName,d);
    
      strcat(fileName, ".xml");
      actualGame = load_game(fileName);
      if(actualGame == NULL)
      {
	changeDisplay(d,    
	TEXT,2,5,10,"Invalid filename or corrupt file",NULL, 
	NULL);
	printDisplay(d);
	clearArea(d,5,5,10,strlen("Invalid filename or corrupt file"));
      }
    }
    while(actualGame == NULL);
    return;
}


int
getName(char name[], screenPosType ** d, int height, int width)
{
    int i, validName = INVALID, a, errorStatus=OK;
    while( validName == INVALID && errorStatus==OK)
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
		      errorStatus=changeDisplay(d,height,width,
		      TEXT,2,5,10,"Insert at least 1 character",NULL, 
		      NULL);   
		      if (errorStatus!=OK)
			return ERROR;
		      printDisplay(d,uMargin,lMargin,height,width);
		      cleaninLine(d,height,width,5,width-TORCH_MAX_WIDTH-1);
		      cleaninLine(d,height,width,6,width-TORCH_MAX_WIDTH-1);
                      DELETE_BUFFER;
                      validName = INVALID;
                }
                else if( i == 30)
                {
		      errorStatus=changeDisplay(d,height,width,
		      TEXT,2,5,10,"The name is too long, insert a valid",NULL, 
		      TEXT,2,6,10,"name with less than 31 characters please",NULL,		
		      NULL);   
		      if (errorStatus!=OK)
			return ERROR;
		      printDisplay(d,uMargin,lMargin,height,width);
		      cleaninLine(d,height,width,5,width-TORCH_MAX_WIDTH-1);
		      cleaninLine(d,height,width,6,width-TORCH_MAX_WIDTH-1);
                      DELETE_BUFFER;
                      validName = INVALID;
                }
                else if( name[0] == ' ' || name[i-1] == ' ')
                {
		      errorStatus=changeDisplay(d,height,width,
		      TEXT,2,5,10,"The name must start and end with a number or digit",NULL, 
		      NULL);
		      if (errorStatus!=OK)
			return ERROR;
		      printDisplay(d,uMargin,lMargin,height,width);
		      cleaninLine(d,height,width,5,width-TORCH_MAX_WIDTH-1);
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
			{ 
			   errorStatus=changeDisplay(d,height,width,
			   TEXT,2,5,10,"The name must start and end with a number or digit",NULL, 
			   NULL);
			   if (errorStatus!=OK)
			    return ERROR;
			   printDisplay(d,uMargin,lMargin,height,width);
			   cleaninLine(d,height,width,5,width-TORCH_MAX_WIDTH-1);
			}
                        if (validName)
                          return OK;
                }
        }
}

int
chooseProfession(character_t *player, game_t *actualGame)
{
        int i = 0, errorStatus,aux;
	errorStatus=changeDisplay(d,height,width,
	TEXT,2,5,10,"The available professions are:",NULL,
	TABLE,2,TORCH_HEIGHT+1,10,height-rows-2,width,NULL,
	TEXT,3,TORCH_HEIGHT,10,"PROFESSIONS:",NULL,
	NULL);  
	
	if(errorStatus!=OK)
	  return ERROR;
	
	printDisplay(d,uMargin,lMargin,height,width);

        while( i < actualGame->professions_size )
        {
		errorStatus=changeDisplay(d,height,width,
		TEXT,3,TORCH_HEIGHT+i,10,sprintf("%d - %s\n", i+1, actualGame->professions[i]->name),NULL,
		NULL);  
                printf(
                i++;
        }

        do
        {
                aux = getint("\n Select your desired profession \n");
        }while( aux > i || aux < 0 );
	
        player->professionID = actualGame->professions[aux-1]->ID;
        player->HP = actualGame->professions[aux-1]->minHP + ( (float)(rand())/RAND_MAX ) *
          (actualGame->professions[aux-1]->maxHP - actualGame->professions[aux-1]->minHP);
	player->maxHP = player->HP;
        errorStatus=changeDisplay(d,height,width,
	TEXT,3,TORCH_HEIGHT+i,10,sprintf("%d - %s\n", i+1, actualGame->professions[i]->name),NULL,
	NULL);  
	
	if(errorStatus!=OK)
	  return ERROR;
	  
	printf("%s, you have choosen to be a %s, your initial health points are %d\n",player->name, actualGame->professions[aux-1]->name, player->HP);
	
	
	
	for(;i>0;i--)
	  cleaninLine(d,height,width,TORCH_HEIGHT+i,TORCH_MAX_WIDTH-1,width-TORCH_MAX_WIDTH-1);
	cleaninLine(d,height,width,5,TORCH_MAX_WIDTH-1,width-TORCH_MAX_WIDTH-1);
	cleaninLine(d,height,width,TORCH_HEIGHT,TORCH_MAX_WIDTH-1,width-TORCH_MAX_WIDTH-1);
	
	
        return;
}


/*Checks if the player has been into the actual room, if he hasn't it generates the fiends that there will be in there, and simulate the combat
returnig if player is dead or alive*/
int
enterRoom(character_t *player, room_t *actualRoom, game_t *actualGame, screenPos ** d, int rows, int columns)
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
    if(status==ALIVE)
      findPotion(player);
    
    
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

void
findPotion(player_t *player)
{
 int probability = (25 + ( (float)(rand())/RAND_MAX ) * (10) );
 if((float)(rand())/RAND_MAX )*100 <= probability)
   player->potions++;
 return;
}