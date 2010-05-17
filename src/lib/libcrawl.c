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

/* General includes */

#include "libcrawl.h"
#include "libparse.h"

/* Macros and constants */


/* Static function prototypes */


/* Static functions */

static profession_t *
new_profession_from_gpnode(gpnode_p p)
{
    gpnode_p node;
    profession_t profession;
    if (p == NULL) return NULL;

    profession = malloc(sizeof(profession_t));
    for (node = p->child; node != NULL; node = node->next){
        if (strcmp(node->name, "ID") == 0){
            profession->ID = atoi(child->value);
        } else if (strcmp(node->name, "Nombre") == 0){
            profession->name = child->value;
        } else if (strcmp(node->name, "MinHP") == 0){
            profession->minHP = atoi(child->value);
        } else if (strcmp(node->name, "MaxHP") == 0){
            profession->maxHP = atoi(child->value);
        } else if (strcmp(node->name, "MinDP") == 0){
            profession->minDP = atoi(child->value);
        } else if (strcmp(node->name, "MaxDP") == 0){
            profession->maxDP = atoi(child->value);
        }
    }
    return profession;
}

static enemy_t *
new_enemy_from_gpnode(gpnode_p p)
{
    gpnode_p node;
    enemy_t enemy;

    if (p == NULL) return NULL;

    enemy = malloc(sizeof(enemy_t));
    for (node = p->child; node != NULL; node = node->next){
        if (strcmp(node->name, "ID") == 0){
            enemy->ID = atoi(node->value);
        } else if (strcmp(node->name, "Nombre") == 0){
            enemy->name = node->value;
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
    gate_t gate;

    if (p == NULL) return NULL;
    gate = malloc(sizeof(gate_t));
    for (node = p->child; node != NULL; node = node->next){
        if (strcmp(node->name, "Nombre") == 0){
            gate->name = node->value;
        } else if (strcmp(node->name, "Destino") == 0){
            gate->room_id = atoi(node->value);
        }
    }
    return gate;
}

static void *
extract_gates(room_t * room, gpnode_p gates)
{
    gpnode_p node;
    int size = 0;
    int idx = 0;
    gate_t ** gates;

    for (node = gates->child; node != NULL, node = node->next){
        if (strcmp(node->name, "Puerta")){
            size++;
        }
    }
    gates = calloc(size, sizeof(gate_t *));
    for (node = gates->child; node != NULL, node = node->next){
        if (strcmp(node->name, "Puerta")){
            gates[idx++] = new_gate_from_gpnode(node);
        }
    }

}

static void *
extract_enemy_ids(room_t * room, gpnode enemies)
{
    gpnode_p node;
    int idx = 0;

    for (node = gates->child; node != NULL, node = node->next){
        if (strcmp(node->name, "Cantidad") == 0){
            room->enemies_size = atoi(node->value);
            room->enemy_ids = calloc(room->enemies_size, sizeof(int));
        } else if (strcmp(node->name, "Enemigo") == 0 && \
                strcmp(node->child->name, "ID") == 0){
            room->enemy_ids[idx++] = atoi(node->child->value);
        }
    }
}

static room_t *
new_room_from_gpnode(gpnode_p p)
{
    gpnode_p node;
    room_t room;

    if (p == NULL) return NULL;

    room = malloc(sizeof(room_t));
    for (node = p->child; node != NULL; node = node->next){
        if (strcmp(node->name, "ID") == 0){
            room->ID = atoi(node->value);
        } else if (strcmp(node->name, "Nombre") == 0){
            room->name = node->value;
        } else if (strcmp(node->name, "Descripcion") == 0){
            room->description = node->value;
        } else if (strcmp(node->name, "Puertas") == 0){
            extract_gates(room, node);
        } else if (strcmp(node->name, "Enemigos") == 0){
            extract_enemy_ids(room, node);
        }
    }
    return room;
}

static void
extract_professions(game_t *game, gpnode_p root)
{
    profession_t ** professions = NULL;
    int prof_idx = 0;
    int prof_size = 0;

    while (root != NULL){
        if (professions == NULL){
            if (strcmp(root->name, "Cantidad")){
                prof_size = atoi(root->value);
                proffesions = calloc(prof_size, sizeof(* profession_t));
            }
        } else if (strcmp(root->name, "Profesion")){
            professions[prof_idx++] = new_profession_from_gpnode(root->child);
        }
        root = root->next;
    }
    game->professions = professions;
    game->professions_size = prof_size;
}

static void
extract_important_points(game_t *game, gpnode_p root)
{
    gpnode_p child = root->child;
    if (strcmp(root->name, "PuntosImportantes") == 0){
        if(strcmp(child, "HabitacionInicioID") == 0){
            game->StartRoomID = atoi(root->child->value);
        } else if(strcmp(child, "HabitacionSalidaID") == 0){
            game->ExitRoomID = atoi(root->child->value);
        }
        child = child->next;
    }
}

static void
extract_enemies(game_t, *game, gpnode root)
{
    enemy_t ** enemies = NULL;
    int enem_idx = 0;
    int enem_size = 0;

    while (root != NULL){
        if (professions == NULL){
            if (strcmp(root->name, "Cantidad")){
                enem_size = atoi(root->value);
                enemies = calloc(enem_size, sizeof(enemy_t *));
            }
        } else if (strcmp(root->name, "Enemigo")){
            enemies[enem_idx++] = new_enemy_from_gpnode(root->child);
        }
        root = root->next;
    }
    game->enemies = enemies;
    game->enemies_size = enem_size;
}

static void
extract_rooms(game_t *game, gpnode_p root)
{
    room_t ** rooms = NULL;
    int room_idx = 0;
    int room_size = 0;

    while (root != NULL){
        if (professions == NULL){
            if (strcmp(root->name, "Cantidad")){
                room_size = atoi(root->value);
                rooms = calloc(room_size, sizeof(room_t *));
            }
        } else if (strcmp(root->name, "Habitacion")){
            rooms[room_idx++] = new_room_from_gpnode(root->child);
        }
        root = root->next;
    }
    game->rooms = rooms;
    game->rooms_size = room_size;
}

/*
 *  Public functions
 */

void
logmsg(logbook_t book, const char * action)
{
    log_entry_t new_entry;
    new_entry->action = action;
    time(new_entry->time);
    new_entry->next = book->log;
    book->log = new_entry;
}

room_t *
open_gate(game_t *g, gate_t *d)
{
    return NULL;
}


game_t *
load(char *filename)
{
    FILE *file;
    game_t * game_p;
    gpnode_p root;
    int line, col;

    file = fopen(filename, "r");
    if (file == NULL) return NULL;

    root = parse(file, &line, &col);

    extract_important_points(game_p, root);
    extract_professions(game_p, root);
    extract_enemies(game_p, root);
    extract_rooms(game_p, root);

    gpn_free(root);
    fclose(file);
    return game_p;
}


int
save(game_t *g)
{
    return 0;
}
