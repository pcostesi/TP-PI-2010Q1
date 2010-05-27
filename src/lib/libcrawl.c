/*
 *      libcrawl.c
 *
 *      Copyright 2010:
 *          Sebastián Maio <email@goes.here>
 *          Juan Pablo Rey <email@goes.here>
 *          Pablo Alejandro Costesich <pcostesi@alu.itba.edu.ar>
 *
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions are
 *      met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following disclaimer
 *        in the documentation and/or other materials provided with the
 *        distribution.
 *      * Neither the name of the ITBA nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 *      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define LIBCRAWL 0.1

/*
 *  System includes
 */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/* General includes */

#include "libcrawl.h"
#include "libparse.h"

/* Macros and constants */

/* Static function prototypes */
static profession_t *  new_profession_from_gpnode(gpnode_p);
static enemy_t *        new_enemy_from_gpnode(game_t *, gpnode_p);
static gate_t *         new_gate_from_gpnode(gpnode_p);
static room_t *         new_room_from_gpnode(gpnode_p);
static unsigned int    extract_gates(room_t *, gpnode_p);
static unsigned int    extract_enemy_ids(room_t *, gpnode_p);
static unsigned int    extract_professions(game_t *, gpnode_p);
static unsigned int    extract_important_points(game_t *, gpnode_p);
static unsigned int    extract_enemies(game_t *, gpnode_p);
static unsigned int    extract_rooms(game_t *, gpnode_p);
static character_t *   load_character(gpnode_p node);

static void alloc_dmg_table(game_t *, int);
static void free_dmg_table(game_t *);

static void             load_enemies(room_t *, gpnode_p);
static void             load_room(game_t *, gpnode_p);
static void             load_rooms(game_t *, gpnode_p);


static gpnode_p    save_rooms(game_t *);
static gpnode_p    save_character(logbook_t *);
static gpnode_p    log_to_node(logbook_t *);

static char *    int2str(int);
static int    nlen(int);
/* Static functions */


static int
nlen(int in)
{
    int size = 0;
    if (in == 0)
        size = 1;
    for (; in != 0; size++, in /= 10)
        if (in < 0)
            in = in * -10;
    return size;
}

static char *
int2str(int in)
{
    char * out;
    int size = nlen(in);
    out = malloc(size + 1);
    sprintf(out, "%d", in);
    return out;
}


static void
load_enemies(room_t * room, gpnode_p node)
{
    gpnode_p enemy;
    int size = 0;
    int * vector;
    if (gpn_cmp_tag(node, "Enemies")){
        for (enemy = gpn_child(node); enemy != NULL; enemy = gpn_next(enemy))
            size++;
        vector = malloc(sizeof(int) * size);
        room->enemies_size = size;
        free(room->enemy_ids);
        size = 0;
        for (enemy = gpn_child(node); enemy != NULL; enemy = gpn_next(enemy))
            if (gpn_cmp_tag(enemy, "EnemyID"))
                vector[size++] = atoi(gpn_get_content(enemy));
        room->enemy_ids = vector;
    }
}

static void
load_room(game_t * g, gpnode_p room)
{
    gpnode_p node, enemies;
    int id, visited;
    if (gpn_cmp_tag(room, "Room")){
        for (node = gpn_child(room); node != NULL; node = gpn_next(node)){
            if (gpn_cmp_tag(node, "ID"))
                id = atoi(gpn_get_content(node));
            else if (gpn_cmp_tag(node, "Visited"))
                visited = atoi(gpn_get_content(node));
            else if (gpn_cmp_tag(node, "Enemies"))
                enemies = node;
        }
        if (id < g->rooms_size){
            g->rooms[id]->visited = visited;
            load_enemies(g->rooms[id], enemies);
        }
    }
}

static void
load_rooms(game_t * g, gpnode_p rooms)
{
    gpnode_p room;

    for (room = gpn_child(rooms); room != NULL; room = gpn_next(room))
        load_room(g, room);
}


static character_t *
load_character(gpnode_p node)
{
    character_t * c = NULL;
    if (node != NULL && gpn_cmp_tag(node, "Character")){
        c = malloc(sizeof(character_t));
        for (node = gpn_child(node); node != NULL; node = gpn_next(node)){
            if (gpn_cmp_tag(node, "Name")){
                c->name = dupstr(gpn_get_content(node));
            } else if (gpn_cmp_tag(node, "HP")){
                c->HP = atoi(gpn_get_content(node));
            } else if (gpn_cmp_tag(node, "RoomID")){
                c->roomID = atoi(gpn_get_content(node));
            } else if (gpn_cmp_tag(node, "ProfessionID")){
                c->professionID = atoi(gpn_get_content(node));
            }
        }
    }
    return c;
}

static gpnode_p
save_character(logbook_t * log)
{
    gpnode_p node, character;

    character = gpn_alloc();
    node = new_gpn_child(character);
    gpn_set_tag(character, dupstr("Character"));

    gpn_set_tag(node, dupstr("Name"));
    gpn_set_content(node, dupstr(log->player->name));

    node = new_gpn_child(character);
    gpn_set_tag(node, dupstr("HP"));
    gpn_set_content(node, int2str(log->player->HP));

    node = new_gpn_child(character);
    gpn_set_tag(node, dupstr("RoomID"));
    gpn_set_content(node, int2str(log->player->roomID));

    node = new_gpn_child(character);
    gpn_set_tag(node, dupstr("ProfessionID"));
    gpn_set_content(node, int2str(log->player->professionID));

    return character;
}

static gpnode_p
save_rooms(game_t * g)
{
    gpnode_p rooms, enemy, enemies, node, room, quantity;
    int iter_enemies, iter_rooms;

    rooms = gpn_alloc();
    gpn_set_tag(rooms, dupstr("Rooms"));
    for (iter_rooms = 0; iter_rooms < g->rooms_size; iter_rooms++){
        room = new_gpn_child(rooms);
        gpn_set_tag(room, dupstr("Room"));
        node = new_gpn_child(room);
        gpn_set_tag(node, dupstr("ID"));
        gpn_set_content(node, int2str(g->rooms[iter_rooms]->ID));
        node = new_gpn_child(room);
        gpn_set_tag(node, dupstr("Visited"));
        gpn_set_content(node, int2str(g->rooms[iter_rooms]->visited));

        enemies = new_gpn_child(room);
        gpn_set_tag(enemies, dupstr("Enemies"));

        quantity = new_gpn_child(enemies);
        gpn_set_tag(quantity, dupstr("Quantity"));
        gpn_set_content(quantity, int2str(g->rooms[iter_rooms]->enemies_size));

        for (iter_enemies = 0; iter_enemies < \
                g->rooms[iter_rooms]->enemies_size; iter_enemies++){

            enemy = new_gpn_child(enemies);
            gpn_set_tag(enemy, dupstr("EnemyID"));
            gpn_set_content(enemy, \
                    int2str(g->rooms[iter_rooms]->enemy_ids[iter_enemies]));
        }

    }
    return rooms;
}



static gpnode_p
log_to_node(logbook_t * book)
{
    gpnode_p root;
    gpnode_p node;
    gpnode_p time_node;
    gpnode_p action_node;
    log_entry_t * entry;
    int exitval = 0;
    char timestring[21];

    root = new_gpn_child(NULL);
    if (root == NULL) return NULL;
    gpn_set_tag(root, dupstr("Log"));

    node = new_gpn_child(root);
    gpn_set_tag(node ,dupstr("Seed"));
    gpn_set_content(node, int2str(book->seed));

    if (book->filename != NULL){
        node = new_gpn_child(root);
        gpn_set_tag(node, dupstr("GameFile"));
        gpn_set_content(node, dupstr(book->filename));
    }

    if (book->player != NULL){
        node = save_character(book);
        gpn_link_as_child(root, node);
    }

    for (entry = book->log; entry != NULL && exitval == 0;
            entry = entry->next){
        node = new_gpn_child(root);
        gpn_set_tag(node, dupstr("Entry"));

        time_node = new_gpn_child(node);
        gpn_set_tag(time_node, dupstr("Time"));
        strftime(timestring, 20, "%d/%m/%Y %H:%M:%S", gmtime(&entry->time));
        gpn_set_content(time_node, dupstr(timestring));

        action_node = new_gpn_child(node);
        gpn_set_tag(action_node, dupstr("Action"));
        gpn_set_content(action_node, dupstr(entry->action));
    }
    return root;
}



static profession_t *
new_profession_from_gpnode(gpnode_p p)
{
    gpnode_p node;
    profession_t *profession;
    if (p == NULL) return NULL;
    profession = calloc(1, sizeof(profession_t));
    if (profession == NULL) return NULL;
    for (node = gpn_child(p); node != NULL; node = gpn_next(node)){
        if (gpn_cmp_tag(node, "ID")){
            profession->ID = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "Nombre")){
            profession->name = dupstr(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MinHP")){
            profession->minHP = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MaxHP")){
            profession->maxHP = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MinDP")){
            profession->minDP = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MaxDP")){
            profession->maxDP = atoi(gpn_get_content(node));
        }
    }
    return profession;
}

static enemy_t *
new_enemy_from_gpnode(game_t * game, gpnode_p p)
{
    gpnode_p node;
    enemy_t * enemy;
    int n;

    if (p == NULL) return NULL;

    enemy = malloc(sizeof(enemy_t));

    for (node = gpn_child(p); node != NULL; node = gpn_next(node)){
        if (gpn_cmp_tag(node, "ID")){
            enemy->ID = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "Nombre")){
            enemy->name = dupstr(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MinHP")){
            enemy->minHP = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MaxHP")){
            enemy->maxHP = atoi(gpn_get_content(node));
        } else if (gpn_ncmp_tag(node, "MinDP-", 6)){
            if ((n = atoi(strrchr(gpn_get_tag(node), '-') + 1)) < \
                    game->professions_size && enemy->ID < game->enemies_size){
                game->dmg_table[enemy->ID][n][0] = atoi(gpn_get_content(node));
            }
        } else if (gpn_ncmp_tag(node, "MaxDP-", 6)){
            if ((n = atoi(strrchr(gpn_get_tag(node), '-') + 1)) < \
                    game->professions_size && enemy->ID < game->enemies_size){
                game->dmg_table[enemy->ID][n][1] = atoi(gpn_get_content(node));
            }

        }
    }
    return enemy;
}

static gate_t *
new_gate_from_gpnode(gpnode_p p)
{
    gpnode_p node;
    gate_t * gate;

    if (p == NULL) return NULL;
    gate = malloc(sizeof(gate_t));

    for (node = gpn_child(p); node != NULL; node = gpn_next(node)){
        if (gpn_cmp_tag(node, "Nombre")){
            gate->name = dupstr(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "Destino")){
            gate->room_id = atoi(gpn_get_content(node));
        }
    }
    return gate;
}

static unsigned int
extract_gates(room_t * room, gpnode_p gates)
{
    int exitval = 0;
    gpnode_p node;
    int size = 0;
    int idx = 0;
    for (node = gpn_child(gates); node != NULL; node = gpn_next(node)){
        if (gpn_cmp_tag(node, "Puerta")){
            size++;
        }
    }
    if (size){
        room->gates = calloc(size, sizeof(gate_t *));
        for (node = gpn_child(gates); node != NULL && exitval == 0;
            node = gpn_next(node), idx++){
            if (gpn_cmp_tag(node, "Puerta"))
                room->gates[idx] = new_gate_from_gpnode(node);
        }

    } else
        room->gates = NULL;
    room->gates_size = size;
    return exitval;
}

static unsigned int
extract_enemy_ids(room_t * room, gpnode_p enemies)
{
    gpnode_p node;
    int idx = 0;
    int exitval = 0;

    room->enemies_size = 0;
    room->enemy_ids = NULL;
    for (node = gpn_child(enemies); node != NULL; node = gpn_next(node)){
        if (gpn_cmp_tag(node, "Cantidad")){
            room->enemies_size = atoi(gpn_get_content(node));
            room->enemy_ids = calloc(room->enemies_size, sizeof(int));
        } else if (gpn_cmp_tag(node, "Enemigo") && \
                gpn_cmp_tag(gpn_child(node), "ID")){
            room->enemy_ids[idx++] = atoi(gpn_get_content(gpn_child(node)));
        } else {
            exitval = 1;
        }
    }
    return exitval;
}

static room_t *
new_room_from_gpnode(gpnode_p p)
{
    gpnode_p node;
    room_t *room;

    if (p == NULL) return NULL;
    room = malloc(sizeof(room_t));
    room->visited = 0;
    room->gates = NULL;
    room->gates_size = 0;
    room->enemy_ids = NULL;
    room->enemies_size = 0;
    for (node = gpn_child(p); node != NULL; node = gpn_next(node)){
        if (gpn_cmp_tag(node, "ID")){
            room->ID = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "Nombre")){
            room->name = dupstr(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "Descripcion")){
            room->description = dupstr(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "Puertas")){
            extract_gates(room, node);
        } else if (gpn_cmp_tag(node, "Enemigos")){
            extract_enemy_ids(room, node);
        }
    }
    return room;
}

static unsigned int
extract_professions(game_t * game, gpnode_p root)
{
    profession_t ** professions = NULL;
    profession_t * profession;
    int prof_size = 0;
    int exitval = 0;
    if (gpn_cmp_tag(root, "Profesiones")){
        for(root = gpn_child(root); root != NULL && exitval == 0;
            root = gpn_next(root)){
            if (professions == NULL){
                if (gpn_cmp_tag(root, "Cantidad")){
                    prof_size = atoi(gpn_get_content(root));
                    professions = calloc(prof_size, sizeof(profession_t *));
                } else exitval = 1;
            } else if (gpn_cmp_tag(root, "Profesion")){
                profession = new_profession_from_gpnode(root);
                if (profession != NULL && profession->ID < prof_size)
                    professions[profession->ID] = profession;
                else{
                    free(profession);
                    exitval = 1;
                }
            }
        }
        game->professions = professions;
        game->professions_size = prof_size;
    }
    return exitval;
}

static unsigned int
extract_important_points(game_t * game, gpnode_p root)
{
    int exitval = 0;
    gpnode_p child;

    if (gpn_cmp_tag(root, "PuntosImportantes")){
        for(child = gpn_child(root); child != NULL; child = gpn_next(child)){
            if(gpn_cmp_tag(child, "HabitacionInicioID")){
                game->StartRoomID = atoi(gpn_get_content(child));
            } else if(gpn_cmp_tag(child, "HabitacionSalidaID")){
                game->ExitRoomID = atoi(gpn_get_content(child));
            } else {
                exitval = 1;
            }
        }
    }
    return exitval;
}

static void
alloc_dmg_table(game_t * game, int enem_size)
{
    int itr;
    game->dmg_table = calloc(enem_size, sizeof(int *));
    for (enem_size-- ;enem_size >= 0; enem_size--){
        itr = game->professions_size;
        game->dmg_table[enem_size] = \
            calloc(game->professions_size, sizeof(int *));
        for (itr--; itr >= 0; itr--){
            game->dmg_table[enem_size][itr] = \
                calloc(2, sizeof(int));
        }
    }
}

static void
free_dmg_table(game_t * game)
{
    int itr, itr2;
    for (itr = 0; itr < game->enemies_size; itr++){
        for (itr2 = 0; itr2 < game->professions_size; itr2++){
            free(game->dmg_table[itr][itr2]);
        }
        free(game->dmg_table[itr]);
    }
    free(game->dmg_table);

}

static unsigned int
extract_enemies(game_t * game, gpnode_p root)
{
    int exitval = 0;
    gpnode_p node;
    enemy_t ** enemies = NULL;
    enemy_t * enemy;
    int enem_size = 0;
    if (gpn_cmp_tag(root, "Enemigos")){
        for (node = gpn_child(root); node != NULL; node = gpn_next(node)){
            if (enemies == NULL){
                if (gpn_cmp_tag(node, "Cantidad")){
                    enem_size = atoi(gpn_get_content(node));
                    enemies = calloc(enem_size, sizeof(enemy_t *));
                    alloc_dmg_table(game, enem_size);
                }
            }
        }

        enem_size = 0;

        for (node = gpn_child(root); node != NULL; node = gpn_next(node)){
            if (gpn_cmp_tag(node, "Enemigo")){
                enemy = new_enemy_from_gpnode(game, node);
                if (enemy != NULL){
                    enemies[enem_size++] = enemy;
                }
            }
        }
    }
    game->enemies = enemies;
    game->enemies_size = enem_size;
    return exitval;
}

static unsigned int
extract_rooms(game_t * game, gpnode_p root)
{
    int exitval = 0;
    room_t ** rooms = NULL;
    room_t * room;
    int room_size = 0;

    if (gpn_cmp_tag(root, "Laberinto")){
        for(root = gpn_child(root); root != NULL && exitval == 0;
            root = gpn_next(root)){
            if (rooms == NULL){
                if (gpn_cmp_tag(root, "Cantidad")){
                    room_size = atoi(gpn_get_content(root));
                    rooms = calloc(room_size, sizeof(room_t *));
                }
            } else if (gpn_cmp_tag(root, "Habitacion")){
                room = new_room_from_gpnode(root);
                if (room != NULL && room->ID < room_size)
                    rooms[room->ID] = room;
                else {
                    exitval = 1;
                    free(room);
                }
            }
        }
    }
    game->rooms = rooms;
    game->rooms_size = room_size;
    return exitval;
}

/*
 *  Public functions
 */

logbook_t *
logmsg(logbook_t * book, const char * action)
{
    log_entry_t * new_entry;
    if (book == NULL){
        book = malloc(sizeof(logbook_t));
        if (book == NULL) return NULL;
        book->log = NULL;
        book->seed = -1;
    }

    new_entry = malloc(sizeof(log_entry_t));
    new_entry->action = action;
    time(&new_entry->time);
    new_entry->next = book->log;
    book->log = new_entry;
    return book;
}

void
free_logbook(logbook_t * book)
{
    log_entry_t * entry, * aux;
    for (entry = book->log; entry != NULL;){
        aux = entry;
        entry = entry->next;
        free(aux);
    }
    free(book);

}


int
log_to_disk(logbook_t * book, const char * filename)
{
    FILE * fp;
    gpnode_p root;
    int exitval = 0;

    root = log_to_node(book);
    if (root != NULL){
        fp = fopen(filename, "w");
        gpn_to_file(fp, root);
        fclose(fp);
        gpn_free(root);
    } else
        exitval = 1;
    return exitval;
}

game_t *
load_game(const char *filename)
{
    FILE *file;
    game_t * game_p = NULL;
    gpnode_p root, node, importantPoints, professions, enemies, rooms = NULL;
    root = NULL;
    game_p = malloc(sizeof(game_t));
    file = fopen(filename, "r");
    if (file == NULL) return NULL;

    root = parse(file, NULL, NULL);
    if (root == NULL){
        fclose(file);
        return NULL;
    }
    if (gpn_cmp_tag(root, "Juego")){
        for (node = gpn_child(root); node != NULL; node = gpn_next(node)){
            if (gpn_cmp_tag(node, "PuntosImportantes")){
                importantPoints = node;
            } else if (gpn_cmp_tag(node, "Profesiones")){
                professions = node;
            } else if (gpn_cmp_tag(node, "Enemigos")){
                enemies = node;
            } else if (gpn_cmp_tag(node, "Laberinto")){
                rooms = node;
            }
        }
        extract_important_points(game_p, importantPoints);
        extract_professions(game_p, professions);
        extract_enemies(game_p, enemies);
        extract_rooms(game_p, rooms);
    } else {
        game_p = NULL;
    }
    gpn_free(root);
    fclose(file);
    return game_p;
}


void
free_game(game_t * game)
{
    free_dmg_table(game);
    free_professions(game);
    free_enemies(game);
    free_rooms(game);
    free(game);
}

void
free_professions(game_t * game)
{
    int itr;
    profession_t * profession = NULL;

    for (itr = 0; itr < game->professions_size; itr++){
        profession = game->professions[itr];
        if (profession != NULL){
            free(profession->name);
            free(profession);
        }
    }
    free(game->professions);

}

void
free_enemies(game_t * game)
{
    int itr;
    enemy_t * enemy = NULL;

    for (itr = 0; itr < game->enemies_size; itr++){
        enemy = game->enemies[itr];
        if (enemy != NULL){
            free(enemy->name);
            free(enemy);
        }
    }
    free(game->enemies);

}

void
free_rooms(game_t * game)
{
    int itr, itr2;
    room_t * room = NULL;
    gate_t * gate = NULL;

    for (itr = 0; itr < game->rooms_size; itr++){
        room = game->rooms[itr];
        if (room != NULL){
            for (itr2 = 0; itr2 < room->gates_size; itr2++){
                gate = room->gates[itr2];
                if (gate != NULL){
                    free(gate->name);
                    free(gate);
                }
            }
            free(room->gates);
            free(room->name);
            free(room->description);
            free(room->enemy_ids);
            free(room);
        }
    }
    free(game->rooms);
}

game_t *
load_state(const char * filename, logbook_t * logbook)
{
    game_t * g = NULL;
    gpnode_p node, root, character, seed, filenode = NULL, rooms;
    FILE *fp;

    fp = fopen(filename, "r");
    root = parse(fp, NULL, NULL);
    fclose(fp);
    if (gpn_cmp_tag(root, "State")){
        for (node = gpn_child(root); node != NULL; node = gpn_next(node)){
            if (gpn_cmp_tag(node, "Character")){
                character = node;
            } else if (gpn_cmp_tag(node, "Seed")){
                seed = node;
            } else if (gpn_cmp_tag(node, "File")){
                filenode = node;
            } else if (gpn_cmp_tag(node, "Rooms")){
                rooms = node;
            }
        }
        g = load_game(gpn_get_content(filenode));
        logbook->seed = atoi(gpn_get_content(seed));
        logbook->player = load_character(character);
        load_rooms(g, rooms);

    } else {
        g = NULL;
    }
    gpn_free(root);
    return g;

}

void
save_state(game_t * g, logbook_t * log, const char * filename)
{
    gpnode_p root, seed, rooms, file, character;
    FILE *fp;

    /* Set up the root node */
    root = gpn_alloc();
    gpn_set_tag(root, dupstr("State"));

    character = save_character(log);
    gpn_link_as_child(root, character);

    file = new_gpn_child(root);
    gpn_set_tag(file, dupstr("File"));
    gpn_set_content(file, dupstr(log->filename));

    seed = new_gpn_child(root);
    gpn_set_tag(seed, dupstr("Seed"));
    gpn_set_content(seed, int2str(log->seed));

    rooms = save_rooms(g);
    gpn_link_as_child(root, rooms);

    fp = fopen(filename, "w");
    gpn_to_file(fp, root);
    fclose(fp);

    gpn_free(root);
}
