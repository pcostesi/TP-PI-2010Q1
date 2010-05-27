/*
 *      libcrawl.h
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
 *      LIMITED TO, PROCUREMENT OF SUBSTARTTAGITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STARTTAGRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define CRAWLER_VERSION 0.1

#ifndef __LIB_CRAWLER
#define __LIB_CRAWLER 1

/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 *  General includes
 */

#include "libparse.h"

/*
 *  Macros and constants
 */

typedef struct Profession{
    int     ID;
    char *  name;
    int     minHP;
    int     maxHP;
    int     minDP;
    int     maxDP;
} profession_t;


typedef struct Enemy{
    int     ID;
    char *  name;
    int     minHP;
    int     maxHP;
} enemy_t;


typedef struct Gate{
    int     room_id;
    char *  name;
} gate_t;


typedef struct Room{
    int         ID;
    char *      name;
    char *      description;
    gate_t **   gates;
    int         gates_size;
    int *       enemy_ids;
    int         enemies_size;
    int         visited;
} room_t;


typedef struct Game{
    int                 StartRoomID;
    int                 ExitRoomID;
    profession_t ** professions;
    int                 professions_size;
    enemy_t **          enemies;
    int                 enemies_size;
    int ***             dmg_table;
    room_t **           rooms;
    int                 rooms_size;
} game_t;


typedef struct Character{
    int     roomID;
    int     professionID;
    int     HP;
    char *  name;
} character_t;



typedef struct LogEntry{
    time_t              time;
    const char *        action;
    struct LogEntry *   next;
} log_entry_t;


typedef struct{
    character_t *   player;
    log_entry_t *   log;
    int             seed;
    char *          filename;
} logbook_t;

/*
 *  Public function prototypes
 */

logbook_t * logmsg(logbook_t *, const char *);
game_t *        load_game(const char *);
int             log_to_disk(logbook_t *, const char *);
void            free_game(game_t *);
void            free_professions(game_t *);
void            free_enemies(game_t *);
void            free_rooms(game_t *);
void            free_logbook(logbook_t *);
void            save_state(game_t *, logbook_t *, const char *);
game_t *            load_state(const char *, logbook_t *);

#endif
