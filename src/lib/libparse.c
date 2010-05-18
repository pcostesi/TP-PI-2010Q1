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

static char * buffer(char);

/* Public function prototypes */

/* Static functions */


/*
 * Function buffer -- string convenience function for function parse.
 *
 * It allocates, stores and trims a string while receiving input from
 * the state machine. This is a static function because there's no need
 * from others to use it, neither it should be accessed by anyone else.
 *
 * It receives any non-zero char and stores it. It returns a pointer to
 * the null-terminated, resized, stack or NULL in case of error.
 *
 * To reset the stack you should call it with the null-char ('\0' or 0).
 * Be advised, if you don't check your input this function may be
 * subject to buffer flushing attacks!
 *
 * One key feature of this function is the fact that it aliviates the
 * burden of keeping most of the error checking and repetitive resizing
 * operations inside a 'transparent' buffer.
 */
static char *
buffer(char c)
{
    char * tmp_p = NULL;
    static char * buf = NULL;
    static int buf_size = 0;
    static int c_size = 0;
    static int ns_idx = 0;

    if (buf_size <= c_size + 1){
        buf_size += 16;
        if (buf == NULL){
            tmp_p = malloc(buf_size);
        } else {
            tmp_p = realloc(buf, buf_size);
        }
        if (tmp_p == NULL){
            free(buf);
            return NULL;
        }
        buf = tmp_p;
        tmp_p = NULL;
    }
    
    buf[c_size] = c;
    c_size++;

    if (c == 0) {
        if (c_size == 1 || ns_idx == 0){
            free(buf);
            tmp_p = NULL;
        } else {
            tmp_p = realloc(buf, ns_idx + 1);
            if (tmp_p != NULL) tmp_p[ns_idx] = 0;
            else tmp_p = buf;
        }
        buf_size = 0;
        buf = NULL;
        ns_idx = 0;
        c_size = 0;
        return tmp_p;
    } else if (isspace(c) == 0) {
        ns_idx = c_size;
    }

    return buf;
}

/* Public functions */

gpnode_p
gpn_alloc(void)
{
    gpnode_p node = calloc(1, sizeof(gpnode_t));

    if (node == NULL){
        exit(255);
    }
    return node;
}

void
gpn_free(gpnode_p node)
{
    gpnode_p tmp;

    while (node != NULL){
        tmp = node;
        node = tmp->next;
        gpn_free(tmp->child);
        free(tmp->value);
        free(tmp->name);
        free(tmp);
    }
}

gpnode_p
child(gpnode_p node)
{    
    gpnode_p aux = gpn_alloc();
    
    if (node != NULL){
        aux->parent = node;
        if (node->child != NULL){
            aux->prev = node->child->prev;
            aux->prev->next = aux;
            node->child->prev = aux;
        } else {
            aux->prev = aux;
            node->child = aux;
        }
    }

    return aux;
}


/*
 * Function parse -- parses an xml-like file into a g.p. tree.
 *
 * This function reads data from a FILE stream and parses it using a
 * simple one-way, non-recursive state machine.
 */
gpnode_p
parse(FILE *stream, int *lp, int *cp)
{
    /*
     * In order not to repeat code and to warn against non-compliant
     * files (whatever that is, since there's no formal definition of
     * the file structure -- although anything not resembling xml should
     * make this cry foul), CLEANUP is a macro that handles the error
     * message and does some pointer-freeing (to be fixed).
     */
    #define CLEANUP     if (lp != NULL) *lp = line;         \
                        if (cp != NULL) *cp= col - 1;       \
                        gpn_free(root);                      \
                        free(buffer(0));                     \
                        return NULL;

    char *endtag;
    gpnode_p node = NULL;
    gpnode_p root = NULL;
    static enum States {STAG, ETAG, DATA, WHITESPACE } state;
    int input, line = 0, col = 0;
    state = WHITESPACE;

    /* This is the *only* explicit loop. Realloc may increase the
     * worst-case complexity of this function, but only for >16 char
     * words. */
    while ((input = fgetc(stream)) != EOF){

        /* Count lines and cols. */
        if(input == '\n'){
            col = 0;
            line++;
        } else
            col++;

        /* Main dispatcher -- Input (and not state) driven */
        switch(input){
            
            case '<':
                /* In case we already have a node, save the buffered
                 * contents to the node and reset the buffer. */
                if (node != NULL){
                    node->value = buffer(0);
                } else {
                    /* We check if the buffer holds any non-whitespace
                     * character, which would mean we're parsing a
                     * non-standard file and we should complain. */
                    endtag = buffer(0);
                    if (endtag != NULL){
                        free(endtag);
                        CLEANUP
                    }
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
                    endtag = buffer(0);
                    if (strcmp(node->name, endtag) == 0){
                        free(endtag);
                        if (node->parent == NULL){
                            return node;
                        } else {
                            node = node->parent;
                        }
                    } else {
                        free(endtag);
                        CLEANUP
                    }
                } else if (state == STAG){
                    node = child(node);
                    if (root == NULL) root = node;
                    node->name = buffer(0);
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

int
gpn_to_file(FILE *stream, gpnode_p root)
{
    #define INDENT  for(iter = 0; iter < indent_level; iter++) \
                    putc('\t', stream);
                    
    static int indent_level = 0;
    int iter;
    int parsed = 0;
    
    while (root != NULL){
        INDENT; printf("<%s>\n", root->name);
        indent_level++;
        if (root->value != NULL){
            INDENT;
            printf("%s\n", root->value);
        }
        parsed += gpn_to_file(stream, root->child);
        indent_level--;
        INDENT; printf("</%s>\n", root->name);
        parsed++;
        root = root->next;
    }
    
    #undef INDENT
    return parsed;
}
