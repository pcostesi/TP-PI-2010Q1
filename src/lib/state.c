/*
 *      state.c
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

#ifndef _SAVESTATE
#define _SAVESTATE 1
/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 *  General includes
 */

#include "state.h"
#include "libcrawl.h"
#include "strings.h"
#include "libparse.h"

/*
 *  Macros and constants
 */

typedef struct LogEntry{
    time_t              time;
    char *        action;
    struct LogEntry *   next;
} log_entry_t;


typedef struct logbook_s{
    character_t *   player;
    log_entry_t *   log;
    int             seed;
    char *          filename;
} logbook_t;


/*
 *  Static function prototypes
 */

static void             load_enemies(room_t *, gpnode_p);
static void             load_room(game_t *, gpnode_p);
static void             load_rooms(game_t *, gpnode_p);
static character_t *   load_character(gpnode_p node);
static gpnode_p    save_rooms(game_t *);
static gpnode_p    save_character(logbook);
static gpnode_p    log_to_node(logbook);


/*
 *  Static functions
 */


static gpnode_p
log_to_node(logbook book)
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
        getRoomByID(g, id)->visited = visited;
        load_enemies(getRoomByID(g, id), enemies);
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
save_character(logbook book)
{
    gpnode_p node, character;

    character = gpn_alloc();
    node = new_gpn_child(character);
    gpn_set_tag(character, dupstr("Character"));

    gpn_set_tag(node, dupstr("Name"));
    gpn_set_content(node, dupstr(book->player->name));

    node = new_gpn_child(character);
    gpn_set_tag(node, dupstr("HP"));
    gpn_set_content(node, int2str(book->player->HP));

    node = new_gpn_child(character);
    gpn_set_tag(node, dupstr("RoomID"));
    gpn_set_content(node, int2str(book->player->roomID));

    node = new_gpn_child(character);
    gpn_set_tag(node, dupstr("ProfessionID"));
    gpn_set_content(node, int2str(book->player->professionID));
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


/*
 *  Public functions
 */

game_t *
load_state(const char * filename, logbook book)
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
        book->seed = atoi(gpn_get_content(seed));
        book->player = load_character(character);
        load_rooms(g, rooms);

    } else {
        g = NULL;
    }
    gpn_free(root);
    return g;

}

void
save_state(game_t * g, logbook log, const char * filename)
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


logbook
logmsg(logbook book, char * action)
{
    log_entry_t * new_entry;
    if (book != NULL){
        new_entry = malloc(sizeof(log_entry_t));
        new_entry->action = action;
        time(&new_entry->time);
        new_entry->next = book->log;
        book->log = new_entry;
    }
    return book;
}

void
free_logbook(logbook book)
{
    log_entry_t * entry, * aux;
    for (entry = book->log; entry != NULL;){
        aux = entry;
	free(entry->action);
        entry = entry->next;
        free(aux);
    }
    free(book);
}


int
log_to_disk(logbook book, const char * filename)
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


logbook
Logbook(int seed, character_t * character, char * filename)
{
    logbook book = malloc(sizeof(logbook_t));
    if (book == NULL) return NULL;
    book->log = NULL;
    book->seed = seed;
    book->filename = filename;
    book->player = character;
    return book;
}

void
setCharacter(logbook book, character_t * player)
{
    book->player = player;
}

character_t *
getCharacter(logbook book)
{
    return book->player;
}

#endif
