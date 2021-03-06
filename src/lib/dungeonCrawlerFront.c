#include "dungeonCrawlerBack.h"
#include "screen.h"

#include <stdarg.h>

#define MENU 3
#define HEALTHBAR 5
#define ENEMYBAR 4
#define BACKGROUND 0
#define DIALOG 6
#define COMBATLOG 2
#define DESCRIPTION 1
#define MAX_LAYERS 7
#define CLEAN_BUFFER while (getchar() != '\n')
#define MESSAGE_SIZE 141
#define DELETE_BUFFER while(getchar() != '\n')
#define ALIVE 1
#define DEAD 0
#define PAUSE while(getchar() != '\n')
#define INVALID 0
#define VALID 1
#define WON 2
#define MAX_INPUT 32
#define NAME_TOO_SHORT 4
#define NAME_TOO_LONG 2
#define BAD_NAME_FORMAT 3


#define LAYER(A, B) (A->layers[B])
#define DIALOG_WIDTH 0.75
#define DIALOG_HEIGHT 0.25
#define CENTER_OFFSET(A, B) (int)(((A) - (B)) / 2.0)
#define GAUGE_SIZE 35

#define COMBAT_HIT_MESSAGE "%s has been hit for %d, leaving him with %d " \
               "health points remaining.\n \nPlease press " \
               "enter to continue."

typedef struct CommandLineUserInterface
{
    screen scr;
    layer * layers; /* *Ordered*, NULL-terminated list of layers */
} GUI_t;

typedef GUI_t * GUI;

void pack(GUI g);
void info(GUI g, const char *s);
int optMenu(GUI g, const char *t, const char ** opts, logbook log);
layer healthBar(void);
layer dialog(void);
void setGaugeBarName(layer l, const char * name);
layer enemyBar(void);layer backgroundLayer(void);
layer menuLayer(void);
layer descriptionLayer(void);
layer combatLogLayer(void);
void splashscreen(GUI g);
character_t * avatarNamescreen(GUI g);
game_t * askForGame(GUI g, char * fileName, logbook log);
void chooseProfessionScreen(GUI g, character_t *player, game_t *currentGame);
void setupNormalLayout(GUI g);
int combat(GUI g, character_t *player, enemy_t *enemy, profession_t *profession, game_t * currentGame, logbook log);
int enterRoom(GUI g, character_t *player, room_t *actualRoom, game_t *currentGame, logbook log);
GUI initgui(void);
void endgui(GUI g);
int menu(GUI g, game_t **currentGame, character_t *player, logbook log);
int getDoor(GUI g, room_t * room);
void endScreen(GUI gui, int status);
int main(int argcount, char ** vector_of_strings);
void drink(GUI g, character_t * player);
void dumpActions(GUI g, logbook log);
int getName(char name[], int len);

    int
main(int argcount, char ** vector_of_strings)
{

    char fileName[36];
    int status = ALIVE, aux;
    int seed = -1;
    character_t *player;
    room_t *currentRoom;
    game_t * currentGame;
    logbook log;
    GUI gui;
    setbuf(stdout, NULL);
    /* This GUI is inspired on the awesome ncurses, which I would be using if
     * it wasn't for its lack of ANSI-C Compliance. */
    gui = initgui();

    splashscreen(gui);

    if (argcount == 1)
    {
        currentGame = askForGame(gui, fileName, NULL);
        seed = time(NULL);
    }
    else
    {
        strcpy(fileName, vector_of_strings[1]);
        currentGame = load_game(fileName);
        if (currentGame == NULL){
            /* Check for the easter egg */
            if (strcmp("cthulhu", fileName))
                info(gui, NULL);
            else
                info(gui, "Invalid name");
            exit(14);
        }
        if (argcount == 2)
        {
            seed = time(NULL);
        }
        else
        {
            seed = atoi(vector_of_strings[2]);
        }
    }
    srand(seed);

    player = avatarNamescreen(gui);

    log = Logbook(seed, player, fileName);

    chooseProfessionScreen(gui, player, currentGame);

    setupNormalLayout(gui);

    currentRoom = getRoomByID(currentGame, currentGame->StartRoomID);
    status = enterRoom(gui, player, currentRoom, currentGame, log);
    while( status == ALIVE )
    {
        status = menu(gui, &currentGame, player, log);
        if(status == ALIVE)
        {
            aux = getDoor(gui, currentRoom);
            status = enterRoom(gui, player, getRoomByID(currentGame, currentRoom->gates[aux]->room_id), currentGame, log);
            currentRoom = getRoomByID(currentGame, currentRoom->gates[aux]->room_id);
        }
    }

    endScreen(gui, status);

    endgui(gui);
    free(player->name);
    free(player);
    free_game(currentGame);
    free_logbook(log);

    return EXIT_SUCCESS;
}



    int
getName(char name[], int len)
{
    int i = 0, validName = VALID, a;
    char buff;
    while((buff = getchar()) != '\n' && buff != EOF && validName != NAME_TOO_LONG)
    {
        if( i >= len)
        {
            validName = NAME_TOO_LONG;
            DELETE_BUFFER;
        } else {
            name[i] = buff;
            i++;
        }
    }
    name[i] = '\0';

    if( i == 0)
    {
        validName = NAME_TOO_SHORT;
    }
    else if( name[0] == ' ' || name[i-1] == ' ')
    {
        validName = BAD_NAME_FORMAT;
    }
    else
    {
        for(a = 0; a < i && validName ; a++)
        {
            if(!(isalpha(name[a]) || name[a] == ' ' || isdigit(name[a])) )
                validName = INVALID;
        }
    }
    return validName;
}




    void
pack(GUI g)
{
    update(g->scr, g->layers);
}

    void
info(GUI g, const char *s)
{
    layer l = LAYER(g, DIALOG);
    static const char *m[] = { \
        "", \
            "  /\\", \
            " /!!\\", \
            " ----", \
            NULL};
    setMode(l, LYR_AUTO_WRAP | LYR_DRAW_MARGINS | LYR_DRAW_TITLE);
    if (s != NULL)
    {
        centerText(l, s);
        draw(l, m);
    } else {
        /* This is a small tribute to Lovecraft */
        centerText(l, "Ph'nglui mglw'nafh Cthulhu R'lyeh\n"
                  "wgah'nagl fhtagn");
    }
    pack(g);
    PAUSE;
    setMode(l, LYR_HIDDEN);
    pack(g);
}

    int
optMenu(GUI g, const char *t, const char ** opts, logbook log)
{
    int nopts = 0, num = 0, exit = 0;
    char n[MAX_INPUT];
    while (opts[nopts++] != NULL);
    setMode(LAYER(g, MENU), LYR_NORMAL | LYR_NO_AUTO_RESIZE);
    setMode(LAYER(g, DIALOG), LYR_HIDDEN);
    vmenu(LAYER(g, MENU), t, opts);
    pack(g);
    do
    {
        if ( scanf("%d",&num) != 1)
        {
            if (scanf("dumpActions %32s.txt", n) != 1){
                putc(0, stdout);
                CLEAN_BUFFER;
                info(g, "Incorrect data type (a number is required)");
            } else if (log != NULL) {
                log_to_disk(log, n); /* haha pun log(n) */
                info(g, "Log saved");
            } else {
                info(g, "You can't save here!");
            }
        }
        else if (num > 0 && num < nopts) {
            putc(0, stdout);
            CLEAN_BUFFER;
            exit = 1;
        } else {
            putc(0, stdout);
            CLEAN_BUFFER;
            info(g, "Incorrect option");
        }
    } while (!exit);
    setMode(LAYER(g, MENU), LYR_HIDDEN);
    return num - 1;
}

    layer
healthBar(void)
{
    layer l = gaugeWidget("Your Health", 35);
    absMoveLayer(l, 1, 2);
    return l;
}

    layer
dialog(void)
{
    int x = ENV_WIDTH * DIALOG_WIDTH, y = ENV_HEIGHT * DIALOG_HEIGHT;
    layer l = newLayer(x, y, (ENV_WIDTH - x) / 2, (ENV_HEIGHT - y) / 2);
    setTitle(l, "Dialog");
    return l;
}

    void
setGaugeBarName(layer l, const char * name)
{
    setTitle(l, name);
}

    layer
enemyBar(void)
{
    layer l = gaugeWidget("Enemy Health", 35);
    absMoveLayer(l, (ENV_WIDTH / 2) * 0.75, (ENV_HEIGHT / 2) * 0.50);
    return l;
}

    layer
backgroundLayer(void)
{

    layer l;
    /* Center the layer */
    l = newLayer(ENV_WIDTH, ENV_HEIGHT, 0, 0);
    centerText(l, "Welcome to DUNGEON CRAWLER\nPress INTRO to START.\n \n \n(This game is powered by UNIX-Fu)");
    setMode(l, 0);
    return l;
}

    layer
menuLayer(void)
{
    int x = ENV_WIDTH * 0.75, y = ENV_HEIGHT * 0.5;
    layer l = newLayer(x, y, (ENV_WIDTH - x) / 2, (ENV_HEIGHT - y) / 2);
    setTitle(l, "Menu");
    return l;
}

    layer
descriptionLayer(void)
{
    layer l = newLayer(20, 5, 0, 0);
    setTitle(l, "Room");
    return l;
}

    layer
combatLogLayer(void)
{
    layer l = newLayer(20, 5, 0, 0);
    setTitle(l, "Combat Log");
    return l;
}

    void
splashscreen(GUI g)
{
    int idx = 1;
    while (g->layers[idx] != NULL){
        setMode(g->layers[idx++], LYR_HIDDEN);
    }
    pack(g);
    PAUSE;
}

    character_t *
avatarNamescreen(GUI g)
{
    character_t * player;
    char name[MAX_INPUT];
    int input = VALID;
    layer dialogL = LAYER(g, DIALOG);
    setMode(dialogL, LYR_DRAW_MARGINS | LYR_DRAW_TITLE | LYR_AUTO_WRAP);
    centerText(dialogL, "Insert the name of your character \n \n(make it " \
            "nice and short!)");
    pack(g);
    while ((input = getName(name, MAX_INPUT)) != VALID){
        switch(input){
            case NAME_TOO_SHORT:
                centerText(dialogL, "I'll call you Empty-Name-man!\nDo you like it?" \
                        "\nWell, choose another name...\n(you're boring)");
                break;
            case NAME_TOO_LONG:
                centerText(dialogL, "Don't you think bards will have\na hard time"\
                        " remembering your name?\nWhy don't you pick "\
                        "a new one?");
                break;
            case INVALID:
            case BAD_NAME_FORMAT:
                centerText(dialogL, "That doesn't look like a name to me...");
                break;
        }
        pack(g);
    }
    player = createCharacter(name);
    setMode(dialogL, LYR_HIDDEN);
    return player;
}


    game_t *
askForGame(GUI g, char * fileName, logbook log)
{
    game_t * currentGame = NULL;
    layer dialog = LAYER(g, DIALOG);
    setMode(LAYER(g, BACKGROUND), LYR_HIDDEN);
    setMode(dialog, LYR_DRAW_MARGINS | LYR_DRAW_TITLE | LYR_AUTO_WRAP);

    /* We do this  instead of copy-pasting the function. */
    if (log == NULL)
        centerText(dialog, "Insert the FULL filename (EXT. INCL.) and press INTRO");
    else
        centerText(dialog, "Insert the savestate name and press INTRO");
    do
    {
        pack(g);
        getName(fileName, MAX_INPUT);
        if (log == NULL)
            currentGame = load_game(fileName);
        else
            currentGame = load_state(fileName, log);
        if(currentGame == NULL){
            centerText(dialog, "Invalid file or corrupt data. \nMake sure you typed it right!");
        }
    } while( currentGame == NULL);
    return currentGame;
}


    void
chooseProfessionScreen(GUI g, character_t *player, game_t *currentGame)
{
    int i = 0, aux;
    char t[141];
    layer life = LAYER(g, HEALTHBAR);
    layer dialogL = LAYER(g, DIALOG);
    /* We make an array of char pointers to already allocated pointers */
    const char ** opts = calloc(sizeof(char *), currentGame->professions_size + 1);
    for (i = 0; i < currentGame->professions_size; i++){
        if (currentGame->professions[i] != NULL)
            opts[i] = currentGame->professions[i]->name;
        else
            opts[i] = NULL;
    }
    opts[i] = NULL;
    sprintf(t, "%s, you may choose among these professions:", player->name);
    aux = optMenu(g, t, opts, NULL);
    free(opts);
    player->professionID = currentGame->professions[aux]->ID;
    player->HP = currentGame->professions[aux]->minHP + ( (float)(rand())/RAND_MAX ) *
        (currentGame->professions[aux]->maxHP - currentGame->professions[aux]->minHP);
    player->maxHP = player->HP;
    gaugeWidgetUpdate(life, 100);
    setMode(life, LYR_DRAW_MARGINS | LYR_DRAW_TITLE);
    setGaugeBarName(life, player->name);
    sprintf(t, "%s, you have chosen to be a %s. Your initial health points are %d", \
        player->name, currentGame->professions[aux]->name, player->HP);
    setMode(dialogL, LYR_NORMAL);
    text(dialogL, t);
    pack(g);
    setMode(dialogL, LYR_HIDDEN);
    PAUSE;
}

    void
setupNormalLayout(GUI g)
{
    layer description = LAYER(g, DESCRIPTION);
    layer menuL = LAYER(g, MENU);
    layer combatlog = LAYER(g, COMBATLOG);
    int x, y;

    getScreenDimensions(g->scr, &x, &y);

    setMode(LAYER(g, DIALOG), LYR_HIDDEN);
    resizeLayer(LAYER(g, DIALOG), x * 0.75, y * 0.25);
    absMoveLayer(LAYER(g, DIALOG), x * 0.125, y * 0.375);

    resizeLayer(description, x / 2 - 2, y - 6);
    absMoveLayer(description, 1, 5);
    setMode(description, LYR_NORMAL);

    resizeLayer(combatlog, x - 2, y / 2 - 2);
    absMoveLayer(combatlog, 1, y / 2 - 1);
    setMode(combatlog, LYR_NORMAL | LYR_HIDDEN);

    resizeLayer(menuL, x / 2 - 3, y - 6);
    absMoveLayer(menuL, x / 2 + 2, 5);
    setTitle(menuL, "Your options...");
    setMode(menuL, LYR_NORMAL | LYR_NO_AUTO_RESIZE);

    setMode(LAYER(g, BACKGROUND), LYR_HIDDEN);

}

    void
setCombatViewport(GUI g)
{
    int t = LYR_NO_AUTO_RESIZE | LYR_NORMAL;
    layer enemyL = LAYER(g, ENEMYBAR);
    layer health = LAYER(g, HEALTHBAR);
    layer combatlog = LAYER(g, COMBATLOG);
    layer description = LAYER(g, DESCRIPTION);
    layer dialog = LAYER(g, DIALOG);
    setMode(combatlog, t);
    setMode(health, t);
    setMode(enemyL, t);
    setMode(dialog, t);
    setMode(description, LYR_HIDDEN);
}

/*Simulates the combat between the player and any posible enemy*/
    int
combat( GUI g, character_t *player, enemy_t *enemy, \
    profession_t *profession, game_t * currentGame, logbook log)
{
    int ret = -1, turn, hit, enemyHP, fullEnemyHP;
    int dp[2];
    layer enemyL = LAYER(g, ENEMYBAR);
    layer combatlog = LAYER(g, COMBATLOG);
    float starter = ((float)(rand())/(RAND_MAX));
    char message[MESSAGE_SIZE];

    setCombatViewport(g);

    setTitle(enemyL, enemy->name);
    gaugeWidgetUpdate(enemyL, 100);

    getEnemyMinAndMaxDPByProfession(currentGame, enemy, profession, dp);

    /* Randomly set the turn */
    starter < 0.5 ? (turn = 0) : (turn = 1);

    enemyHP = (enemy->minHP + ( (float)(rand())/RAND_MAX ) * \
            (enemy->maxHP - enemy->minHP));
    fullEnemyHP = enemyHP;

    /* Clean up the combatlog */
    text(combatlog, "");
    /* Warn the user about the upcoming battle */
    sprintf(message, "In the room you encounter a %s.\n(Who is not "
            "willing to let you pass...)\nHe has %d HP", \
            enemy->name, enemyHP);
    info(g, message);

    while (player->HP > 0 && enemyHP > 0)
    {
        if(turn % 2 == 0)
        {
            hit = damageRoll(profession->maxDP, profession->minDP);
            enemyHP -= hit;
            if (enemyHP < 0) enemyHP = 0;
            sprintf(message, COMBAT_HIT_MESSAGE, enemy->name, hit, enemyHP);
        }
        else
        {
            hit = damageRoll(dp[0], dp[1]);
            player->HP -= hit;
            if (player->HP < 0) player->HP = 0;
            sprintf(message, COMBAT_HIT_MESSAGE, player->name, hit, \
                    player->HP);
        }
        logmsg(log, strlen(message) + 1, message);
        centerText(LAYER(g, COMBATLOG), message);
        gaugeWidgetUpdate(enemyL, enemyHP * 100 / fullEnemyHP);
        gaugeWidgetUpdate(LAYER(g, HEALTHBAR), \
                player->HP * 100 / player->maxHP);
        pack(g);
        PAUSE;
        turn = (turn + 1) % 2;
    }

    if (player->HP <= 0)
    {
        sprintf(message, "You have been defeated, and thus have "
                "dishonored your family");
        ret = DEAD;
    }
    else
    {
        sprintf(message, "You have crushed another foolish enemy "
                "that got in your way");
        ret = ALIVE;
    }
    logmsg(log, strlen(message) + 1, message);
    centerText(LAYER(g, COMBATLOG), message);
    pack(g);
    PAUSE;
    setMode(combatlog, LYR_HIDDEN);
    setMode(enemyL, LYR_HIDDEN);
    setMode(LAYER(g, DESCRIPTION), LYR_NORMAL | LYR_NO_AUTO_RESIZE);
    return ret;
}



/*Checks if the player has been into the actual room, if he hasn't
  it generates the fiends that  there will be in there, and simulate the combat
  returnig if player is dead or alive*/
    int
enterRoom(GUI g, character_t *player, room_t *actualRoom, game_t *currentGame, logbook log)
{
    int status = ALIVE, i;
    layer description = LAYER(g, DESCRIPTION);
    layer menuL = LAYER(g, MENU);
    enemy_t * enemy;
    char message[MESSAGE_SIZE];

    player->roomID = actualRoom->ID;

    setMode(LAYER(g, DIALOG), LYR_HIDDEN);
    setMode(description, LYR_AUTO_WRAP | LYR_DRAW_MARGINS | LYR_DRAW_TITLE);
    setMode(menuL, LYR_HIDDEN);
    text(description, actualRoom->description);
    setTitle(description, actualRoom->name);

    sprintf(message, "You're standing in room %s (%d)", \
            actualRoom->name, actualRoom->ID);
    logmsg(log, strlen(message) + 1, message);

    if (actualRoom->visited == 0)
    {
        for(i = actualRoom->enemies_size - 1; status == ALIVE && i >= 0; i--)
        {
            enemy = getEnemyByID(currentGame, actualRoom->enemy_ids[i]);
            status = combat(g, player, enemy, \
                    getProfessionByID(currentGame, \
                        player->professionID), currentGame, log);
            if (getPotion(player)){
                info(g, "The enemy has dropped a potion!");
            }
            actualRoom->enemies_size--;
        }
        actualRoom->visited = 1;
    }
    if(actualRoom->ID == currentGame->ExitRoomID && status != DEAD)
        status = WON;
    return status;
}

    GUI
initgui(void)
{
    GUI g = malloc(sizeof(GUI_t));
    if (g != NULL){
        g->scr = minitscr(stdout, ENV_WIDTH, ENV_HEIGHT, SCR_NEWLINE);
        g->layers = malloc(sizeof(layer) * (MAX_LAYERS + 1));
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

void endgui(GUI g)
{
    int itr = 0;
    if (g != NULL){
        endscr(g->scr);
        while (g->layers[itr] != NULL){
            freeLayer(g->layers[itr++]);
        }
        free(g->layers);
    }
    free(g);
}

    void
drink(GUI g, character_t * player)
{
    char message[141];
    setMode(LAYER(g, DESCRIPTION), LYR_HIDDEN);
    if (drinkPotion(player) == -1){
        info(g, "But you're out of potions!");
    } else {
        gaugeWidgetUpdate(LAYER(g, HEALTHBAR), \
                player->HP * 100 / player->maxHP);
        sprintf(message, "Ok! Now you have %-10d Health points"
                " and %-10d Potions left.", player->HP, player->potions);
        info(g, message);
    }
    setMode(LAYER(g, DESCRIPTION), LYR_NORMAL);
}

void
dumpActions(GUI g, logbook log){
    layer dialogL = LAYER(g, DIALOG);
    char name[MAX_INPUT];
    setMode(LAYER(g, DESCRIPTION), LYR_HIDDEN);
    setMode(dialogL, LYR_AUTO_WRAP | LYR_DRAW_MARGINS | LYR_DRAW_TITLE);
    centerText(dialogL, "Type the name of the file and press ENTER");
    pack(g);
    getName(name, MAX_INPUT);
    log_to_disk(log, name);
    info(g, "Dump saved");
    setMode(dialogL, LYR_HIDDEN);
    setMode(LAYER(g, DESCRIPTION), LYR_NORMAL);
}
/*Displays a menu with the actions that the player can take*/
    int
menu(GUI g, game_t **currentGame, character_t *player, logbook log)
{
    game_t * newGame = NULL;
    char name[MAX_INPUT];
    layer dialogL = LAYER(g, DIALOG);
    layer menuL = LAYER(g, MENU);
    int opt = 0;
    do
    {
        /* We only need to create this once */
        static const char * opts[] = {"Switch rooms", \
            "Drink a potion", \
                "Save State...", \
                "Load State...", \
                "Quit game", \
                "Dump Actions...", \
                NULL };
        opt = optMenu(g, "Your options are:", opts, log);
        setMode(LAYER(g, ENEMYBAR), LYR_HIDDEN);
        setMode(LAYER(g, DIALOG), LYR_HIDDEN);
        switch(++opt)
        {
            case 1 :
                return ALIVE;
                break;
            case 2 :
                drink(g, player);
                break;
            case 3 :
                setMode(dialogL, LYR_NORMAL);
                setMode(LAYER(g, DESCRIPTION), LYR_HIDDEN);
                centerText(dialogL, "Type the name of the file and press ENTER");
                pack(g);
                getName(name, MAX_INPUT);
                save_state(*currentGame, log, name );
                setMode(dialogL, LYR_HIDDEN);
                setMode(LAYER(g, DESCRIPTION), LYR_NORMAL);
                break;
            case 4 :
                newGame = askForGame(g, name, log);
                if (newGame != NULL){
                    free(*currentGame);
                    *currentGame = newGame;
                    info(g, "A new state has been loaded.");
                } else {
                    info(g, "This is not a valid savestate.");
                }
                break;
            case 5 :
                return DEAD;
                break;
            case 6 :
                dumpActions(g, log);
                break;
        }

    } while(1);
    setMode(menuL, LYR_HIDDEN);
    return DEAD;
}

    int
getDoor(GUI g, room_t * room)
{

    int itr;
    const char ** opts = malloc(sizeof(char *) * (room->gates_size + 1));
    for (itr = 0; itr < room->gates_size; itr++){
        if (room->gates[itr] != NULL && room->gates[itr]->name != NULL)
            opts[itr] = room->gates[itr]->name;
        else
            opts[itr] = "Invalid option";
    }
    opts[itr] = NULL;

    setMode(LAYER(g, ENEMYBAR), LYR_HIDDEN);
    setMode(LAYER(g, DIALOG), LYR_HIDDEN);
    itr = optMenu(g, "Your options are:", opts, NULL);
    free(opts);
    return itr;
}

    void
endScreen(GUI gui, int status)
{
    int idx = 0, mode = 0;
    while (gui->layers[idx] != NULL){
        if (idx != BACKGROUND)
            mode = LYR_HIDDEN;
        else
            mode = 0;
        setMode(gui->layers[idx++], mode);
    }
    if(status == DEAD)
        centerText(LAYER(gui, BACKGROUND), \
                "You have been defeated.\n \nShame on you!");
    if(status == WON)
        centerText(LAYER(gui, BACKGROUND), \
                "You have escaped this realm of nightmares!");
    pack(gui);
}
