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

#ifndef LIBPARSE
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
#include "strings.h"

/* Macros and constants */


typedef struct GPNode{
    gpnode_p next, prev;
    gpnode_p child, parent;
    char *value;
    char *name;
} gpnode_t;


/* Public functions */


gpnode_p
gpn_init(gpnode_p node){
    if (node != NULL){
        node->next = NULL;
        node->prev = node;
        node->value = NULL;
        node->name = NULL;
        node->next = NULL;
        node->child = NULL;
    }
    return node;
}

gpnode_p
gpn_alloc(void)
{
    gpnode_p node;
    node = malloc(sizeof(gpnode_t));
    node = gpn_init(node);
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

void
gpn_link_as_child(gpnode_p parent, gpnode_p child)
{
    if (parent != NULL && child != NULL){
        child->parent = parent;
        if (parent->child != NULL){
            child->prev = parent->child->prev;
            child->prev->next = child;
            parent->child->prev = child;
        } else {
            child->prev = child;
            parent->child = child;
        }
    }
}

gpnode_p
new_gpn_child(gpnode_p node)
{
    gpnode_p aux = gpn_alloc();
    if (node != NULL && aux != NULL)
        gpn_link_as_child(node, aux);
    return aux;
}

gpnode_p gpn_next(gpnode_p n)
{
    if (n != NULL)
        n = n->next;
    return n;
}

gpnode_p gpn_prev(gpnode_p n)
{
    if (n != NULL){
        if (n->prev != NULL && gpn_parent(n) != NULL && \
                gpn_child(gpn_parent(n)) == n)
            n = NULL;
        n = n->prev;
    }
    return n;
}

gpnode_p gpn_parent(gpnode_p n)
{
    if (n != NULL) n = n->parent;
    return n;
}

gpnode_p gpn_child(gpnode_p n)
{
    if (n != NULL)
        n = n->child;
    return n;
}

int
gpn_cmp_tag(gpnode_p n, const char * c)
{
    if (n == NULL)
        return 0;
    else
        return strcmp(gpn_get_tag(n), c) == 0;
}

int
gpn_ncmp_tag(gpnode_p n, const char * c, int h)
{
    if (n == NULL)
        return 0;
    else
        return strncmp(gpn_get_tag(n), c, h) == 0;
}

void
gpn_set_content(gpnode_p n, char * c)
{
    if (n != NULL)
        n->value = c;
}

void
gpn_set_tag(gpnode_p n, char * c)
{
    if (n != NULL)
        n->name = c;
}

char *
gpn_get_content(gpnode_p n)
{
    if (n != NULL)
        return n->value;
    else
        return NULL;
}

char *
gpn_get_tag(gpnode_p n)
{
    if (n != NULL)
        return n->name;
    else
        return NULL;
}

/**
 * @brief Function parse -- parses an xml-like file into a g.p. tree.
 *
 * @param stream: the input stream.
 * @param lp: the error line variable (or NULL).
 * @param cp: the error column variable.
 *
 * @return A pointer to the root of the parsed tree or NULL
 * in case of error.
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
                        free(strpop(&context));              \
                        return NULL;

    char *endtag = NULL;
    gpnode_p node = NULL;
    gpnode_p root = NULL;
    static enum States {STAG, ETAG, DATA, WHITESPACE } state;
    int input, line = 0, col = 0;
    string_t context; // = CLEAN_STRING_T;
    strinit(&context);
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
                    gpn_set_content(node, strpop(&context));
                } else {
                    /* We check if the buffer holds any non-whitespace
                     * character, which would mean we're parsing a
                     * non-standard file and we should complain. */
                    endtag = strpop(&context);
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
                    strappend(input, &context);
                }
                break;

            case '>':
                if (state == ETAG){
                    endtag = strpop(&context);
                    if (gpn_cmp_tag(node, endtag)){
                        free(endtag);
                        if (gpn_parent(node) == NULL){
                            return node;
                        } else {
                            node = gpn_parent(node);
                        }
                    } else {
                        free(endtag);
                        CLEANUP
                    }
                } else if (state == STAG){
                    node = new_gpn_child(node);
                    if (node == NULL){
                        CLEANUP
                    } else if (root == NULL)
                        root = node;
                    gpn_set_tag(node, strpop(&context));
                }
                state = WHITESPACE;
                break;

            default:
                if(isspace(input) && input != 0){
                    if (state != WHITESPACE)
                        strappend(input, &context);
                } else if (input != 0){
                    if (state == WHITESPACE)
                        state = DATA;
                    strappend(input, &context);
                }
        }

    }
    return root;

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
        INDENT; fprintf(stream, "<%s>", gpn_get_tag(root));

        if (gpn_child(root) == NULL) {
            if (gpn_get_content(root) != NULL)
                fprintf(stream, "%s", gpn_get_content(root));
        } else {
            indent_level++;
            fprintf(stream, "\n");
            if (gpn_get_content(root) != NULL){
                INDENT;
                fprintf(stream, "%s\n", gpn_get_content(root));
            }
            parsed += gpn_to_file(stream, gpn_child(root));
            indent_level--;
            INDENT;
        }
        fprintf(stream, "</%s>\n", gpn_get_tag(root));
        parsed++;
        root = gpn_next(root);
    }

    #undef INDENT
    return parsed;
}

#endif
