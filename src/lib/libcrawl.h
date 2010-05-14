/*
 *      filename.h
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

/*
 *  Macros and constants
 */

typedef struct profession{
    int ID;
    char *name;
    int minHP, maxHP;
    int minDP, maxDP;
} profession_t;


typedef struct enemy{
    int ID;
    char * name;
    int minHP, maxHP, minDP, maxDP, minDP0, maxDP0;
    int minDP1, maxDP1, minDP2, maxDP2;
} enemy_t;


typedef struct gate{
    int room_id;
    char * name;
} gate_t;


typedef struct room{
    int ID;
    char * name;
    char * description;
    gate_t * gates;
    int gates_size;
    enemy_t * enemies;
    int enemies_size;
} room_t;


typedef struct game{
    int StartRoomID;
    int ExitRoomID;
    profession_t ** professions;
    int professions_size;
    enemy_t ** enemies;
    int enemies_size;
    room_t ** rooms;
    int rooms_size;
} game_t;


typedef struct character{
    int roomID;
    int professionID;
    int HP;
    int DP;
} character_t;



typedef struct log_entry{
    time_t *time;
    char * action;
    struct log_entry *next;
} log_entry_t;


typedef struct logbook{
    char *name;
    character_t *player;
    log_entry_t *log_start;
    log_entry_t *log_end;
} logbook_t;

/*
 *  Static function prototypes
 */

void logmsg(logbook_t book, const char * action, character_t player);

room_t *open_gate(game_t *g, gate_t *d);

/*
 *  Public function prototypes
 */

#endif