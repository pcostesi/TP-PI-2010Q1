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

#ifndef LIBCRAWL
#define LIBCRAWL 0.1

/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* General includes */

#include "libcrawl.h"
#include "libparse.h"
#include "strings.h"
#include "decode.h"

/* Macros and constants */
#define MATCHES_TYPE(_Type, _Elem, _Id) _Type * _object = (_Type *) _Elem; \
                                    if (_object != NULL) { \
                                        return _object->ID == _Id; \
                                    } else { return -1; }


/* Static function prototypes */

static int getXByID(void **, int, int, int (*)(void *, int));
static int matches_enemy_id(void *, int);
static int matches_room_id(void *, int);
static int matches_profession_id(void *, int);


/* Static functions */


/**
 * @brief
 *
 * @param
 *
 * @return
 */
static int
getXByID(void **vector, int max, int key, int (*matches_id)(void *, int))
{
    int itr, idx = -1;
    if (vector != NULL){
        for (itr = 0; itr < max && idx == -1; itr++){
            if ((*matches_id)(vector[itr], key)){
                idx = itr;
            }
        }
    }
    return idx;
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
static int
matches_enemy_id(void * enemy, int id)
{
    MATCHES_TYPE(enemy_t, enemy, id)
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
static int
matches_profession_id(void * p, int id)
{
    MATCHES_TYPE(profession_t, p, id)
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
static int
matches_room_id(void * r, int id)
{
    MATCHES_TYPE(room_t, r, id)
}



/*
 *  Public functions
 */


/**
 * @brief
 *
 * @param
 *
 * @return
 */
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


/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
free_game(game_t * game)
{
    free_professions(game);
    free_enemies(game);
    free_rooms(game);
    free(game);
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
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


/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
free_enemies(game_t * game)
{
    int itr;
    enemy_t * enemy = NULL;

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

}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
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


/**
 * @brief
 *
 * @param
 *
 * @return
 */
enemy_t *
getEnemyByID(game_t * game, int id)
{
    return (enemy_t *) game->enemies[getXByID((void **)game->enemies, \
            game->enemies_size, id, matches_enemy_id)];
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
room_t *
getRoomByID(game_t * game, int id)
{
    return (room_t *) game->rooms[getXByID((void **)game->rooms, \
            game->rooms_size, id, matches_room_id)];
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
profession_t *
getProfessionByID(game_t * game, int id)
{
    return (profession_t *) game->professions[getXByID(\
            (void **)game->professions, \
            game->professions_size, id, \
            matches_profession_id)];
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
int
getProfessionIndexByID(game_t * game, int id)
{
    return getXByID((void **)game->professions, \
            game->professions_size, id, matches_profession_id);
}


#endif
