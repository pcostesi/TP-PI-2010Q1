#include "dungeonCrawlerBack.h"


int
main(int argcount, char ** vector_of_strings)
{

    char fileName[36];
    int status = ALIVE, i, aux;
    game_t * actualGame;
    srand(time(NULL));
    printf("\n\n\nWelcome to DUNGEON CRAWLER \n");
    printf("Press enter to begin the game\n\n");
    PAUSE;
    int seed = 0;
    do
    {
      printf("Insert the name of the source file, without the extension please. \n");
      getName(fileName);
      strcat(fileName, ".xml");
      actualGame = load_game(fileName);
      if(actualGame == NULL)
    printf("Invalid file name or corrupt data. \n");
    } while( actualGame == NULL);

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
    logbook log = Logbook(seed, player, fileName);


    printf("Inster the name of the avatar \n");
    getName(player->name);

    chooseProfession(player, actualGame );
    room_t *actualRoom = getRoomByID(actualGame, actualGame->StartRoomID);
    player->potions=0;
    status = enterRoom( player, actualRoom, actualGame, log);
    while( status == ALIVE )
    {
    status = menu(actualGame, player, log);
        if(status == ALIVE)
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
                status = enterRoom(player, getRoomByID(actualGame, actualRoom->gates[aux-1]->room_id), actualGame, log);
                actualRoom = getRoomByID(actualGame, actualRoom->gates[aux-1]->room_id);
    }
    }
        if(status == DEAD)
                printf("You have been defeated \n");
        if(status == WON)
                printf("You have succesfully escaped the dungeon, congratulations!!\n");
        free(player->name);
        free(player);
    free_game(actualGame);
    free_logbook(log);

    return EXIT_SUCCESS;
}
