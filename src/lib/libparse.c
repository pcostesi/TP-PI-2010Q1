/*
 *      libparse.c
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


#define LIBPARSE 0.1

/*
 *  System includes
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
 *  General includes
 */

#include "libparse.h"

/* Macros and constants */

#define BLOCKSIZE 16

/* Static function prototypes */

/* Public function prototypes */

/* Static functions */

static char *
buffer(char c)
{
    char * tmp_p = NULL;
    static char * buf = NULL;
    static int buf_size = 0;
    static int c_size = 0;
    if (buf_size - 2 < c_size){
        buf_size += BLOCKSIZE;
        if (buf == NULL){
            tmp_p = malloc(buf_size);
        } else {
            tmp_p = realloc(buf, buf_size);
        }
        if (tmp_p == NULL) {return NULL;}
        buf = tmp_p;
    }
    buf[c_size] = c;
    c_size++;
    if (c == '\0') {
        if (c_size == 1){
            c_size = 0;
        }
        tmp_p = realloc(buf, c_size * sizeof(char));
        buf_size = 0;
        buf = NULL;
        c_size = 0;
        return tmp_p;
    }
    return buf;
}

/* Public functions */

gpnode_p
gpn_alloc(void)
{
    gpnode_p node = malloc(sizeof(gpnode_t));
    if (node == NULL){
        printf("OUT OF MEMORY! :(");
        exit(255);
    }
    return node;
}

void
gpn_free(gpnode_p node){
    gpnode_p tmp;
    while (node != NULL){
        tmp = node;
        node = tmp->next;
        free(tmp->value);
        free(tmp->name);
        gpn_free(tmp->child);
        free(tmp);
    }
}

static gpnode_p child(gpnode_p node){
    gpnode_p aux = gpn_alloc();
    if (node != NULL){
        if (node->child != NULL){
            aux->next = node->child;
        }
        node->child = aux;
        aux->parent = node;
    }
    return aux;
}


gpnode_p parse(FILE *stream)
{
    #define CLEANUP     printf("Cleaning up! In line %d\n", __LINE__);\
                        gpn_free(node); \
                        free(buffer(0)); \
                        return NULL;
    gpnode_p node = NULL;
    static enum States {STAG, ETAG, DATA, WHITESPACE } state;
    int input;
    int indent = 0;
    state = WHITESPACE;
    while ((input = fgetc(stream)) != EOF){
        switch(input){
            case '<':
                if (node != NULL){
                    node->value = buffer(0);
                }
                if (state == WHITESPACE || state == DATA){
                    state = STAG;
                } else {
                    CLEANUP
                }
                break;

            case '/':
                if (state == STAG){
                    state = ETAG;
                } else if (state == ETAG){
                    CLEANUP
                } else {
                    buffer(input);
                }
                break;

            case '>':
                if (state == ETAG){
                    if (strcmp(node->name, buffer(0)) == 0){
                        indent--;
                        if (node->parent == NULL){
                            printf("My child is %p\n", node->child);
                            return node;
                        } else {
                            node = node->parent;
                        }
                    } else {
                        CLEANUP
                    }
                } else if (state == STAG){
                    node = child(node);
                    node->name = buffer(0);
                    indent++;
                }
                state = WHITESPACE;
                break;

            default:
                if(isspace(input) && input != 0){
                    if (state != WHITESPACE)
                        buffer(input);
                } else if (input != 0){
                    if (state == WHITESPACE){
                        state = DATA;
                    }
                    buffer(input);
                }
        }
    }
    return node;
    #undef CLEANUP
}
