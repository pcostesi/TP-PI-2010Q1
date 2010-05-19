#include "libparse.h"
#include "libcrawl.h"

#define ALIVE 1
#define DEAD 0
#define PAUSE while(getchar() != '\n')

int combat(character_t *player, enemy_t *enemy, profession_t *profession);
void roomDescription(room_t actualRoom);

/*Checks if the player has been into the actual room, if he hasn't it generates the fiends that  there will be in there, and simulate the combat
returnig if player is dead or alive*/
int 
enterRoom(character_t *player, room_t *actualRoom, game_t actualGame)	
{
	
	int status = 1, i = 0;
	printf("%s te encuentras en la habitacion %s /n", player->name, actualRoom->name);
	roomDescription(*actualRoom);
	if (actualRoom->visited == 0)
	{
		while( status == ALIVE && i < actualRoom->enemies_size)
		{
			status = combat(player, actualGame.enemies[actualRoom->enemy_ids[i]], actualGame.professions[player->professionID]);
		}
		actualRoom->visited = 1;
	}
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
	return;	
}


int
combat(character_t *player, enemy_t *enemy, profession_t *profession)
{
	int winner, turn, hit, enemyHP;
	float starter = (rand()/(RAND_MAX)); 
	starter < 0.5 ? (turn = 0) : (turn = 1);
	enemyHP = enemy->minHP + ( rand()/RAND_MAX ) * (enemy->maxHP - enemy->minHP); 
	printf("In the room you encounter a %s, who isn't willing to let you pass, he has %d HP /n", (enemy->name), enemyHP);
	printf("You engage a %s in battle: /n", enemy->name);
	
	while (player->HP > 0 && enemyHP > 0)
	{
		if(turn % 2 == 0)
		{
			hit = damageRoll(profession->maxDP, profession->minDP);
			enemyHP -= hit;
			printf("%s has been hit for %d, leaving him with %d health point remaining./n Please press enter to continue.", enemy->name, hit, enemyHP);
			PAUSE;
		}
		else
		{
			hit = damageRoll(enemy->maxDP[player->professionID], enemy->minDP[player->professionID]);
			player->HP -= hit;
			printf("%s has been hit for %d, leaving him with %d health point remainin.g/n Please press enter to continue.", player->name, hit, player->HP);
			PAUSE;
		}
	}
	if (player->HP < 0)
	{
		printf("You have been defeated, and thus have dishonored your family /n");		
		return DEAD;
	}
	else
	{
		printf("You have crushed another foolish enemy thata got in your way");
		return ALIVE;	
	}
}





int
damageRoll(int max, int min)
{
	return (min + ( rand()/RAND_MAX ) * (max - min) );
}