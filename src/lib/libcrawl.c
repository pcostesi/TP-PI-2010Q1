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

#define BLOCKSIZE 32

/* Static function prototypes */
static char *           strdup(const char *);
static profession_t *  new_profession_from_gpnode(gpnode_p);
static enemy_t *        new_enemy_from_gpnode(gpnode_p);
static enemy_t *        new_enemy_from_gpnode(gpnode_p);
static gate_t *         new_gate_from_gpnode(gpnode_p);
static room_t *         new_room_from_gpnode(gpnode_p);
static unsigned int    extract_gates(room_t *, gpnode_p);
static unsigned int    extract_enemy_ids(room_t *, gpnode_p);
static unsigned int    extract_professions(game_t *, gpnode_p);
static unsigned int    extract_important_points(game_t *, gpnode_p);
static unsigned int    extract_enemies(game_t *, gpnode_p);
static unsigned int    extract_rooms(game_t *, gpnode_p);

/* Static functions */

static char *
strdup(const char * string)
{
    int itr = 0;
    int size = 0;
    int idx = 0;
    char * tmp;
    char * new = calloc(BLOCKSIZE);
    
    if (new == NULL) return NULL;
    for (; *string != 0; *string++, idx++){
        if (size >= idx){
            size += BLOCKSIZE;
            tmp = realloc(new, size);
            if (tmp == NULL) return NULL;
            new = tmp;
        }
        new[idx] = *string;
    }
    new[idx++] = 0;
    new = realloc(new, idx);
    
    return new; 
}

#undef BLOCKSIZE

static profession_t *
new_profession_from_gpnode(gpnode_p p)
{
    gpnode_p node;
    profession_t *profession;
    if (p == NULL) return NULL;

    profession = malloc(sizeof(profession_t));
    for (node = p->child; node != NULL; node = node->next){
        if (strcmp(node->name, "ID") == 0){
            profession->ID = atoi(node->value);
        } else if (strcmp(node->name, "Nombre") == 0){
            profession->name = node->value;
            node->value = NULL;
        } else if (strcmp(node->name, "MinHP") == 0){
            profession->minHP = atoi(node->value);
        } else if (strcmp(node->name, "MaxHP") == 0){
            profession->maxHP = atoi(node->value);
        } else if (strcmp(node->name, "MinDP") == 0){
            profession->minDP = atoi(node->value);
        } else if (strcmp(node->name, "MaxDP") == 0){
            profession->maxDP = atoi(node->value);
        }
    }
    return profession;
}

static enemy_t *
new_enemy_from_gpnode(gpnode_p p)
{
    gpnode_p node;
    enemy_t * enemy;

    if (p == NULL) return NULL;

    enemy = malloc(sizeof(enemy_t));
    for (node = p->child; node != NULL; node = node->next){
        if (strcmp(node->name, "ID") == 0){
            enemy->ID = atoi(node->value);
        } else if (strcmp(node->name, "Nombre") == 0){
            enemy->name = node->value;
            node->value = NULL;
        } else if (strcmp(node->name, "MinHP") == 0){
            enemy->minHP = atoi(node->value);
        } else if (strcmp(node->name, "MaxHP") == 0){
            enemy->maxHP = atoi(node->value);
        } else if (strcmp(node->name, "MinDP-0") == 0){
            enemy->minDP0 = atoi(node->value);
        } else if (strcmp(node->name, "MaxDP-0") == 0){
            enemy->maxDP0 = atoi(node->value);
        } else if (strcmp(node->name, "MinDP-1") == 0){
            enemy->minDP1 = atoi(node->value);
        } else if (strcmp(node->name, "MaxDP-1") == 0){
            enemy->maxDP1 = atoi(node->value);
        } else if (strcmp(node->name, "MinDP-2") == 0){
            enemy->minDP2 = atoi(node->value);
        } else if (strcmp(node->name, "MaxDP-2") == 0){
            enemy->maxDP2 = atoi(node->value);
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
    for (node = p->child; node != NULL; node = node->next){
        if (strcmp(node->name, "Nombre") == 0){
            gate->name = node->value;
            node->value = NULL;
        } else if (strcmp(node->name, "Destino") == 0){
            gate->room_id = atoi(node->value);
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
    gate_t ** gate_array;

    for (node = gates->child; node != NULL; node = node->next){
        if (strcmp(node->name, "Puerta")){
            size++;
        }
    }
    gate_array = calloc(size, sizeof(gate_t *));
    for (node = gates->child; node != NULL; node = node->next){
        if (strcmp(node->name, "Puerta")){
            gate_array[idx] = new_gate_from_gpnode(node);
            if (gate_array[idx] == NULL) exitval |= (1 << idx);
            idx++;
        }
    }
    return exitval;
}

static unsigned int
extract_enemy_ids(room_t * room, gpnode_p enemies)
{
    gpnode_p node;
    int idx = 0;
    int exitval = 0;
    
    for (node = enemies->child; node != NULL; node = node->next){
        if (strcmp(node->name, "Cantidad") == 0){
            room->enemies_size = atoi(node->value);
            room->enemy_ids = calloc(room->enemies_size, sizeof(int));
        } else if (strcmp(node->name, "Enemigo") == 0 && \
                strcmp(node->child->name, "ID") == 0){
            room->enemy_ids[idx++] = atoi(node->child->value);
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
    for (node = p->child; node != NULL; node = node->next){
        if (strcmp(node->name, "ID") == 0){
            room->ID = atoi(node->value);
        } else if (strcmp(node->name, "Nombre") == 0){
            room->name = node->value;
            node->value = NULL;
        } else if (strcmp(node->name, "Descripcion") == 0){
            room->description = node->value;
            node->value = NULL;
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
    int prof_idx = 0;
    int prof_size = 0;
    int exitval = 0;

    if (strcmp(root->name, "Profesiones") == 0){
        for(root = root->child; root != NULL; root = root->next){
            if (professions == NULL){
                if (strcmp(root->name, "Cantidad") == 0){
                    prof_size = atoi(root->value);
                    professions = calloc(prof_size, sizeof(profession_t *));
                }
            } else if (strcmp(root->name, "Profesion") == 0){
                professions[prof_idx] = new_profession_from_gpnode(root->child);
                if (professions[prof_idx] == NULL) exitval |= (1 << prof_idx);
                prof_idx++;
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
    
    if (strcmp(root->name, "PuntosImportantes") == 0){
        for(child = root->child; child != NULL; child = child->next){
            if(strcmp(child->name, "HabitacionInicioID") == 0){
                game->StartRoomID = atoi(child->value);
            } else if(strcmp(child->name, "HabitacionSalidaID") == 0){
                game->ExitRoomID = atoi(child->value);
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
    int enem_idx = 0;
    int enem_size = 0;

    if (strcmp(root->name, "Enemigos") == 0){
        for (root = root->child; root != NULL; root = root->next){
            if (enemies == NULL){
                if (strcmp(root->name, "Cantidad") == 0){
                    enem_size = atoi(root->value);
                    enemies = calloc(enem_size, sizeof(enemy_t *));
                }
            } else if (strcmp(root->name, "Enemigo") == 0){
                enemies[enem_idx] = new_enemy_from_gpnode(root->child);
                if (enemies[enem_idx] == NULL) exitval |= (1 << enem_idx);
                enem_idx++;
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
    room_t ** rooms = NULL;
    int room_idx = 0;
    int room_size = 0;

    if (strcmp(root->name, "Laberinto") == 0){
        for(root = root->child; root != NULL; root = root->next){
            if (rooms == NULL){
                if (strcmp(root->name, "Cantidad") == 0){
                    room_size = atoi(root->value);
                    rooms = calloc(room_size, sizeof(room_t *));
                }
            } else if (strcmp(root->name, "Habitacion") == 0){
                rooms[room_idx++] = new_room_from_gpnode(root->child);
            }
        }
    }
    game->rooms = rooms;
    game->rooms_size = room_size;
}

/*
 *  Public functions
 */

void
logmsg(logbook_t * book, const char * action)
{
    log_entry_t * new_entry;
    new_entry = malloc(sizeof(log_entry_t));
    new_entry->action = action;
    time(new_entry->time);
    new_entry->next = book->log;
    book->log = new_entry;
}

void
free_logbook(logbook_t * book)
{

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
    
    root = child(NULL);
    if (root == NULL) return 1;
    root->name = strdup("Log");
    root->value = NULL;

    for (entry = book->log; entry != NULL && exitval != 0; entry = entry->next){
        node = child(root);
        node->name = strdup("Entry");
        
        time_node = child(node);
        time_node->name = strdup("Time");
        strftime(timestring, "%d/%m/%Y %H:%M:%S");
        time_hode->value = strdup(timestring);

        action_node = child(node);
        action_node->name = strdup("Action");
        action_node->value = strdup(entry->action);
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
    if (strcmp(root->name, "Juego") != 0) return NULL;
        for (node = root->child; node != NULL; node = node->next){
            if (strcmp(node->name, "PuntosImportantes") == 0){
                extract_important_points(game_p, node);
            } else if (strcmp(node->name, "Profesiones") == 0){
                extract_professions(game_p, node);
            } else if (strcmp(node->name, "Enemigos") == 0){
                extract_enemies(game_p, node);
            } else if (strcmp(node->name, "Laberinto") == 0){
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
        free(profession->name);
        free(profession);
    }
    free(game->professions);
    for (itr = 0; itr < game->enemies_size; itr++){
        enemy = game->enemies[itr];
        free(enemy->name);
        free(enemy);
    }
    free(game->enemies);
    for (itr = 0; itr < game->rooms_size; itr++){
        room = game->rooms[itr];
        for (itr2 = 0; itr2 < room->gates_size; itr2++){
            gate = room->gates[itr2];
            free(gate->name);
            free(gate);
        }
        free(room->name);
        free(room->description);
        free(room->enemy_ids);
        free(room);
    }
    free(game->rooms);
    free(game);
}
