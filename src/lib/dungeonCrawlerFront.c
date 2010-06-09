#include "dungeonCrawlerBack.h"
#include "screen.h"



#define MENU 3
#define HEALTHBAR 5
#define ENEMYBAR 4
#define BACKGROUND 0
#define DIALOG 6
#define COMBATLOG 2
#define DESCRIPTION 1
#define MAX_LAYERS 7

#define LAYER(A, B) (A->layers[B])


typedef struct CommandLineUserInterface
{
    screen scr;
    layer * layers; /* *Ordered*, NULL-terminated list of layers */
} GUI_t;

typedef GUI_t * GUI;

void pack(GUI g)
{
    update(g->scr, g->layers);
}

layer healthBar(void)
{
    layer l = gaugeWidget("Your Health", 35);
    absMoveLayer(l, 1, 1);
    return l;
}

layer dialog(void)
{
    int x = ENV_WIDTH * 0.75, y = ENV_HEIGHT * 0.5;
    layer l = newLayer(x, y, (ENV_WIDTH - x) / 2, (ENV_HEIGHT - y) / 2);
    setTitle(l, "Dialog");
    return l;
}

void setGaugeBarName(layer l, const char * name)
{
    setTitle(l, name);
}

layer enemyBar(void)
{
    layer l = gaugeWidget("Enemy Health", 35);
    absMoveLayer(l, (ENV_WIDTH / 2) * 0.75, (ENV_HEIGHT / 2) * 0.25);
    return l;
}

layer backgroundLayer(void)
{

    layer l;
    /* Center the layer */
    l = newLayer(ENV_WIDTH, ENV_HEIGHT, 0, 0);
    centerText(l, "Welcome to DUNGEON CRAWLER\nPress INTRO to START.\n\n(This game is powered by UNIX-Fu)");
    setMode(l, 0);
    return l;
}

layer menuLayer(void)
{
    int x = ENV_WIDTH * 0.75, y = ENV_HEIGHT * 0.5;
    layer l = newLayer(x, y, (ENV_WIDTH - x) / 2, (ENV_HEIGHT - y) / 2);
    setTitle(l, "Menu");
    return l;
}

layer descriptionLayer(void)
{
    layer l = newLayer(20, 5, 0, 0);
    setTitle(l, "Room");
    return l;
}

layer combatLogLayer(void)
{
    layer l = newLayer(20, 5, 0, 0);
    setTitle(l, "Combat Log");
    return l;
}

void splashscreen(GUI g)
{
    int idx = 1;
    while (g->layers[idx] != NULL){
        setMode(g->layers[idx++], SCR_HIDDEN);
    }
    pack(g);
    PAUSE;
}

character_t * avatarNamescreen(GUI g)
{
    character_t * player;
    char name[MAX_INPUT + 1];
    centerText(LAYER(g, DIALOG), "Insert the name of your character \n\n(make it " \
                    "nice and short! This are Twitter Times!)");
    pack(g);
    printf("\n"); /* Compensate for matrix cursor alignment */
    getName(name, MAX_INPUT);
    player = createCharacter(name);
    return player;
}


game_t * askForGame(GUI g, char * fileName)
{
    game_t * currentGame = NULL;
    layer dialog = LAYER(g, DIALOG);
    setMode(LAYER(g, BACKGROUND), SCR_HIDDEN);
    setMode(dialog, SCR_DRAW_MARGINS | SCR_DRAW_TITLE | SCR_AUTO_WRAP);
    
    centerText(dialog, "Insert the filename without extension and press INTRO");
    do
    {
      pack(g);
      printf("\n"); /* Compensate for cursor position */
      getName(fileName, MAX_INPUT);
      strcat(fileName, ".xml");
      currentGame = load_game(fileName);
      if(currentGame == NULL){        
          centerText(dialog, "Invalid file or corrupt data. Make sure you typed it right!");
      }
    } while( currentGame == NULL);
    return currentGame;
}


void
chooseProfessionScreen(GUI g, character_t *player, game_t *actualGame)
{
        int i = 0, aux;
        char t[141];
        layer life = LAYER(g, HEALTHBAR);
        layer dialog = LAYER(g, DIALOG);
        /* Hackish workaround to get a menu. We make an array of char pointers to already allocated pointers */
        const char ** opts = malloc(sizeof(char *) * sizeof(actualGame->professions_size));
        for (i = 0; i < actualGame->professions_size; i++){
            opts[i] = actualGame->professions[i]->name;
        }
        sprintf(t, "Select your profession, brave %s!%c", player->name, 0);
        dialog = vmenu(dialog, t, opts);
        if (dialog == NULL){
            exit(1);
        }
        pack(g);
        do
        {
                aux = getint("\n");
        }while( aux > i || aux < 0 );
        player->professionID = actualGame->professions[aux-1]->ID;
        player->HP = actualGame->professions[aux-1]->minHP + ( (float)(rand())/RAND_MAX ) *
          (actualGame->professions[aux-1]->maxHP - actualGame->professions[aux-1]->minHP);
        gaugeWidgetUpdate(life, 100);
        setMode(life, SCR_DRAW_MARGINS | SCR_DRAW_TITLE);
        setGaugeBarName(life, player->name);
        sprintf(t, "%s, you have choosen to be a %s. Your initial health points are %d%c",player->name, actualGame->professions[aux-1]->name, player->HP, 0);
        text(dialog, t);
        
        pack(g);
        PAUSE;
}

void
setupNormalLayout(GUI g)
{
    layer description = LAYER(g, DESCRIPTION);
    layer menu = LAYER(g, MENU);
    layer combatlog = LAYER(g, COMBATLOG);
    int x, y;

    setMode(LAYER(g, DIALOG), SCR_HIDDEN);
    
    getScreenDimensions(g->scr, &x, &y);
    resizeLayer(description, x / 2 - 2, y - 6);
    absMoveLayer(description, 1, 5);
    setMode(description, SCR_AUTO_WRAP | SCR_DRAW_MARGINS | SCR_DRAW_TITLE);
    
    resizeLayer(combatlog, x / 2 - 2, y - 6);
    absMoveLayer(combatlog, 1, 5);
    setMode(combatlog, SCR_AUTO_WRAP | SCR_DRAW_MARGINS | SCR_DRAW_TITLE | SCR_HIDDEN);

    resizeLayer(menu, x / 2 - 3, y - 6);
    absMoveLayer(menu, x / 2 + 2, 5);
    setTitle(menu, "Your options...");
    setMode(menu, SCR_AUTO_WRAP | SCR_DRAW_MARGINS | SCR_DRAW_TITLE | SCR_NO_AUTO_RESIZE);
}


/*Checks if the player has been into the actual room, if he hasn't it generates the fiends that  there will be in there, and simulate the combat
returnig if player is dead or alive*/
int
enterRoom(GUI g, character_t *player, room_t *actualRoom, game_t *actualGame, logbook log)
{
    int status = ALIVE, i = 0;
    layer description = LAYER(g, DESCRIPTION);
    layer menu = LAYER(g, MENU);
    player->roomID = actualRoom->ID;
    enemy_t * enemy;
    
    setMode(LAYER(g, DIALOG), SCR_HIDDEN);
    
    setMode(description, SCR_AUTO_WRAP | SCR_DRAW_MARGINS | SCR_DRAW_TITLE);
    text(description, actualRoom->description);
    setTitle(description, actualRoom->name);

    setMode(menu, SCR_AUTO_WRAP | SCR_DRAW_MARGINS | SCR_DRAW_TITLE | SCR_NO_AUTO_RESIZE);
    vmenu(menu, "You may choose from one of the following options:", (const char *[]){"TODO", "draw", "more", "options", NULL});

    pack(g);
    /*

    if (actualRoom->visited == 0)
    {
        while( status == ALIVE && i < actualRoom->enemies_size)
        {
        enemy = getEnemyByID(actualGame, actualRoom->enemy_ids[i]);
            status = combat(player, enemy, getProfessionByID(actualGame, player->professionID), actualGame);
            i++;
        getPotion(player);
        }
        actualRoom->visited = 1;
    }
        if(actualRoom->ID == actualGame->ExitRoomID && status != DEAD)
                status = WON;
     
     */
    return status;
}

GUI initgui(void)
{
    GUI g = malloc(sizeof(GUI_t));

    if (g != NULL){
        g->scr = initscr(stdout, ENV_WIDTH, ENV_HEIGHT);
        g->layers = malloc(sizeof(layer) * MAX_LAYERS + 1);
        g->layers[MENU] = menuLayer();
        g->layers[HEALTHBAR] = healthBar();
        g->layers[ENEMYBAR] = enemyBar();
        g->layers[BACKGROUND] = backgroundLayer();
        g->layers[DIALOG] = dialog();
        g->layers[DESCRIPTION] = descriptionLayer();
        g->layers[COMBATLOG] = combatLogLayer();
        g->layers[MAX_LAYERS] = NULL;
    }
    return g;
}


/*Displays a menu with the actions that the player can take*/
int
menu(GUI g, game_t *actualGame, character_t *player, logbook log)
{
  do
  {
    char name[MAX_INPUT + 1];
    int aux;
    printf("1 - Switch rooms\n");
    printf("2 - Drink a potion\n");
    printf("3 - Save Game...\n");
    printf("4 - Load Game...\n");
    printf("5 - Quit game\n");
    printf("6 - Dump Actions to...\n");

    do
    {
      aux = getint("\n Select an option please \n");
    }while( aux > 7 || aux < 1 );

    switch(aux)
    {
      case 1 : return ALIVE;
      case 2 : drinkPotion(player);
          break;
      case 3 :getName(name, MAX_INPUT);
          strcat(name, ".xml");
          save_state(actualGame, log, name );
          break;
      case 4 :do
          {
          printf("Insert the name of the source file, without the extension please. \n");
          getName(name, MAX_INPUT);
          strcat(name, ".xml");
          actualGame = load_state(name, log);
          if(actualGame == NULL)
          printf("Invalid file name or corrupt data. \n");
          } while( actualGame == NULL);
          break;
      case 5 : return DEAD;
      case 6 : printf("Type the name of the dump and then press ENTER\n"); getName(name, MAX_INPUT); log_to_disk(log, name); break;
     }

  } while(1);
}



int
main(int argcount, char ** vector_of_strings)
{

    char fileName[36];
    int status = ALIVE, i, aux;
    int seed = -1;
    character_t *player;
    room_t *currentRoom;
    game_t * currentGame;
    logbook log;
    /* This GUI is inspired on the awesome ncurses, which I would be using if
     * it wasn't for its lack of ANSI-C Compliance. */
    GUI gui = initgui();

    if (argcount > 1){
        seed = atoi(vector_of_strings[1]);
    } else {
        srand(time(NULL));
    }

    splashscreen(gui);

    currentGame = askForGame(gui, fileName);

    player = avatarNamescreen(gui);

    log = Logbook(seed, player, fileName);

    chooseProfessionScreen(gui, player, currentGame);

    setupNormalLayout(gui);
    
    currentRoom = getRoomByID(currentGame, currentGame->StartRoomID);
    status = enterRoom(gui, player, currentRoom, currentGame, log);
    while( status == ALIVE )
    {
    status = menu(gui, currentGame, player, log);
        if(status == ALIVE)
    {
        printf("In the current room you see %d doors, which one do u wish to take? \n", currentRoom->gates_size );
                do
                {
                        for(i = 0; i < currentRoom->gates_size; i++)
                        {
                                printf("You may choose door number %d, wich is the %s \n", (i+1), currentRoom->gates[i]->name);
                        }
                        aux = getint("Select the desired door \n\n\n");
                } while( aux > i || aux < 0 );
                status = enterRoom(gui, player, getRoomByID(currentGame, currentRoom->gates[aux-1]->room_id), currentGame, log);
                currentRoom = getRoomByID(currentGame, currentRoom->gates[aux-1]->room_id);
    }
    }
        if(status == DEAD)
                printf("You have been defeated \n");
        if(status == WON)
                printf("You have succesfully escaped the dungeon, congratulations!!\n");
        free(player->name);
        free(player);
    free_game(currentGame);
    free_logbook(log);

    return EXIT_SUCCESS;
}
