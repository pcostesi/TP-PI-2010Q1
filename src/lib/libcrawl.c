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
static enemy_t *        new_enemy_from_gpnode(gpnode_p, int);
static gate_t *         new_gate_from_gpnode(gpnode_p);
static room_t *         new_room_from_gpnode(gpnode_p);
static unsigned int    extract_gates(room_t *, gpnode_p);
static unsigned int    extract_enemy_ids(room_t *, gpnode_p);
static unsigned int    extract_professions(game_t *, gpnode_p);
static unsigned int    extract_important_points(game_t *, gpnode_p);
static unsigned int    extract_enemies(game_t *, gpnode_p);
static unsigned int    extract_rooms(game_t *, gpnode_p);

/* Static functions */

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
new_enemy_from_gpnode(gpnode_p p, int prof)
{
    gpnode_p node;
    enemy_t * enemy;

    if (p == NULL) return NULL;

    enemy = malloc(sizeof(enemy_t));

    enemy->maxDP = calloc(prof, sizeof(int));
    enemy->minDP = calloc(prof, sizeof(int));
    enemy->prof = prof;
    for (node = gpn_child(p); node != NULL; node = gpn_next(node)){
        if (gpn_cmp_tag(node, "ID")){
            enemy->ID = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "Nombre")){
            enemy->name = dupstr(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MinHP")){
            enemy->minHP = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MaxHP")){
            enemy->maxHP = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MinDP-", 6)){
            enemy->minDP[atoi(strrchr(gpn_get_tag(node), '-') + 1)] = atoi(gpn_get_content(node));
        } else if (gpn_cmp_tag(node, "MaxDP-", 6)){
            enemy->maxDP[atoi(strrchr(gpn_get_tag(node), '-') + 1)] = atoi(gpn_get_content(node));
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
    for (node = gates->child; node != NULL; node = node->next){
        if (strcmp(node->name, "Puerta") == 0){
            size++;
        }
    }
    if (size){
        room->gates = calloc(size, sizeof(gate_t *));
        for (node = gates->child; node != NULL && exitval == 0;
            node = node->next, idx++){
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
        if (strcmp(node->name, "ID") == 0){
            room->ID = atoi(gpn_get_content(node));
        } else if (strcmp(node->name, "Nombre") == 0){
            room->name = dupstr(gpn_get_content(node));
        } else if (strcmp(node->name, "Descripcion") == 0){
            room->description = dupstr(gpn_get_content(node));
        } else if (strcmp(node->name, "Puertas") == 0){
            extract_gates(room, node);
        } else if (strcmp(node->name, "Enemigos") == 0){
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

static unsigned int
extract_enemies(game_t * game, gpnode_p root)
{
    int exitval = 0;
    enemy_t ** enemies = NULL;
    enemy_t * enemy;
    int enem_size = 0;
    if (gpn_cmp_tag(root, "Enemigos")){
        for (root = gpn_child(root); root != NULL; root = gpn_next(root)){
            if (enemies == NULL){
                if (gpn_cmp_tag(root, "Cantidad")){
                    enem_size = atoi(root->value);
                    enemies = calloc(enem_size, sizeof(enemy_t *));
                }
            } else if (gpn_cmp_tag(root, "Enemigo")){
                enemy = new_enemy_from_gpnode(root, game->professions_size);
                if (enemy != NULL && enemy->ID < enem_size){
                    enemies[enemy->ID] = enemy;
                } else {
                    exitval = 1;
                    free(enemy);
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
    gpnode_p node;
    gpnode_p time_node;
    gpnode_p action_node;
    log_entry_t * entry;
    int exitval = 0;
    char timestring[21];

    fp = fopen(filename, "w");

    root = new_gpn_child(NULL);
    if (root == NULL) return 1;
    gpn_set_tag(root, dupstr("Log"));

    node = new_gpn_child(root);
    gpn_set_tag(node ,dupstr("Seed"));
    sprintf(timestring, "%20d", book->seed);
    gpn_set_content(node, dupstr(timestring));

    if (book->filename != NULL){
        node = new_gpn_child(root);
        gpn_set_tag(node, dupstr("GameFile"));
        gpn_set_value(node, dupstr(book->filename));
    }

    if (book->player != NULL){
        node = new_gpn_child(root);
        gpn_set_tag(node, dupstr("Character"));
        node = new_gpn_child(node);
        gpn_set_name(node, dupstr("Name"));
        gpn_set_content(node, dupstr(book->player->name));
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
    gpn_to_file(fp, root);
    fclose(fp);
    gpn_free(root);

    return exitval;
}

room_t *
open_gate(game_t *g, gate_t *d)
{
    return NULL;
}


game_t *
load_game(const char *filename)
{
    FILE *file;
    game_t * game_p;
    gpnode_p root, node;
    int line, col;

    game_p = malloc(sizeof(game_t));
    file = fopen(filename, "r");
    if (file == NULL) return NULL;

    root = parse(file, &line, &col);
    if (root == NULL){
        fclose(file);
        free(game_p);
        return NULL;
    }
    if (!gpn_cmp_tag(root, "Juego")) return NULL;
        for (node = gpn_child(root); node != NULL; node = gpn_next(node)){
            if (gpn_cmp_tag(node, "PuntosImportantes")){
                extract_important_points(game_p, node);
            } else if (gpn_cmp_tag(node, "Profesiones")){
                extract_professions(game_p, node);
            } else if (gpn_cmp_tag(node, "Enemigos")){
                extract_enemies(game_p, node);
            } else if (gpn_cmp_tag(node, "Laberinto")){
                extract_rooms(game_p, node);
            }
        }
    gpn_free(root);
    fclose(file);
    return game_p;
}


void
free_game(game_t * game)
{
    int itr, itr2;
    profession_t * profession = NULL;
    enemy_t * enemy = NULL;
    room_t * room = NULL;
    gate_t * gate = NULL;

    for (itr = 0; itr < game->professions_size; itr++){
        profession = game->professions[itr];
        if (profession != NULL){
            free(profession->name);
            free(profession);
        }
    }
    free(game->professions);

    for (itr = 0; itr < game->enemies_size; itr++){
        enemy = game->enemies[itr];
        if (enemy != NULL){
            free(enemy->name);
            free(enemy->minDP);
            free(enemy->maxDP);
            free(enemy);
        }
    }
    free(game->enemies);

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
    free(game);
}
