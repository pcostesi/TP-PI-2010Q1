/*
 *      logger.c
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


#ifndef _LOGGER
#define _LOGGER 1

/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *  General includes
 */

#include "libcrawl.h"
#include "strings.h"
#include "libparse.h"
#include "logger.h"



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

typedef logbook_t * logbook;

/*
 *  Static function prototypes
 */

static gpnode_p    log_to_node(logbook_t *);

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

/*
 * Public functions
 */


logbook
logmsg(logbook book, const char * action)
{
    log_entry_t * new_entry;
    if (book == NULL){
        Logbook(-1, NULL, NULL);
    }

    new_entry = malloc(sizeof(log_entry_t));
    new_entry->action = action;
    time(&new_entry->time);
    new_entry->next = book->log;
    book->log = new_entry;
    return book;
}

void
free_logbook(logbook book)
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
    book = malloc(sizeof(logbook_t));
    if (book == NULL) return NULL;
    book->log = NULL;
    book->seed = seed;
    book->filename = filename;
}

#endif
