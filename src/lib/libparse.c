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

/* Static functions */

/*
 * ===================
 * WARNING: DEPRECATED
 * ===================
 *
 * See Context-Preserving String Utils
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
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


/*
 * strinit -- Context initializer for String Utilities.
 *
 * This utility function should be called if you're using the stack to
 * correctly set up the structure (hereon called 'environment carrier').
 *
 */
void
strinit(string_p str)
{
    if (str != NULL){
        str->buffer = NULL;
        str->buffer_size = 0;
        str->char_idx = 0;
        str->last_non_whitespace_idx = 0;
    }

}

/*
 * strfree -- frees any heap-alloc'd Environment Carrier and its buffer.
 */
void
strfree(string_p s)
{
    if (s != NULL)
        free(s->buffer);
    free(s);
}

/*
 * strnew -- heap-allocate a new Environment Carrier.
 *
 * This function implicitly calls strinit, so you don't have to call it.
 */
string_p
strnew(void)
{
    string_p newstring = malloc(sizeof(string_t));
    strinit(newstring);
    return newstring;
}

/*
 * strappend -- push a new character into the buffer.
 *
 * Pushes any non-zero character and echoes. If anything goes wrong
 * (like out-of-memory errors) it returns 0. KEEP IN MIND THAT IF YOU
 * PUSH 0 YOU WILL GET 0.
 */
int
strappend(char c, string_p s)
{
    char * tmp;
    if (s != NULL && c != 0){
        /* Check the buffer size. We add 2 to the last_char size
         * because we need to push a char and add a trailing zero.
         * (for the sake of safety). */
        if (s->buffer == NULL){
            s->buffer = malloc(BLOCKSIZE);
            if (s->buffer == NULL) return 0;
        } else if (s->buffer_size < s->char_idx + 2){
            tmp = realloc(s->buffer, s->buffer_size + BLOCKSIZE);
            if (tmp == NULL) return 0;
            s->buffer = tmp;
            s->buffer_size += BLOCKSIZE;
        }

        /* Set the last non-whitespace char */
        if (!isspace(c))
            s->last_non_whitespace_idx = s->char_idx;
        /* Do the actual push into the string and null-terminate it */
        s->buffer[s->char_idx++] = c;
        s->buffer[s->char_idx] = 0;
    }
    return c;
}

char *
strtrm(string_p s)
{
    char * exitval = NULL;
    if (s != NULL && s->buffer != NULL && \
            s->buffer_size >= s->last_non_whitespace_idx){
        /* Trim buffer and make room to add a trailing zero (sentinel).
         * Since array indices start at zero but sizes start at one, we
         * need to add 2 to the last_non_whitespace_idx. */
        if (s->last_non_whitespace_idx == 0){
            s->buffer = realloc(s->buffer, 1);
            s->buffer[0] = 0;
            s->char_idx = 0;
            s->last_non_whitespace_idx = 0;
            s->buffer_size = 1;
        } else {
            s->buffer = realloc(s->buffer, s->last_non_whitespace_idx + 2);
            s->buffer[s->last_non_whitespace_idx + 1] = 0;
            s->char_idx = s->last_non_whitespace_idx + 1;
            s->buffer_size = s->last_non_whitespace_idx + 2;
        }
        exitval = s->buffer;
    }
    return exitval;
}


/*
 * strpop -- detaches a null-terminated, trimmed string and resets the
 * Environment Carrier.
 *
 * It's your sole responsibility to clean up the detached string.
 */
char *
strpop(string_p s)
{
    char * exitval = NULL;
    if (s != NULL){
        exitval = strtrm(s);
        s->buffer = NULL;
        s->buffer_size = 0;
        s->last_non_whitespace_idx = 0;
        s->char_idx = 0;
    }
    return exitval;
}


void
gpn_init(gpnode_p node){
    if (node != NULL){
        node->next = NULL;
        node->prev = node;
        node->value = NULL;
        node->name = NULL;
        node->next = NULL;
        node->child = NULL;
    }
}

gpnode_p
gpn_alloc(void)
{
    gpnode_p node;
    node = malloc(sizeof(gpnode_t));
    if (node == NULL) return NULL;
    gpn_init(node);
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
                        free(strpop(&context));                     \
                        return NULL;

    char *endtag;
    gpnode_p node = NULL;
    gpnode_p root = NULL;
    static enum States {STAG, ETAG, DATA, WHITESPACE } state;
    int input, line = 0, col = 0;
    state = WHITESPACE;
    string_t context;
    strinit(&context);

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
                    node->value = strpop(&context);
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
                    node->name = strpop(&context);
                }
                state = WHITESPACE;
                break;

            default:
                if(isspace(input) && input != 0){
                    if (state != WHITESPACE){
                        strappend(input, &context);
                    }
                } else if (input != 0){
                    if (state == WHITESPACE){
                        state = DATA;
                    }
                    strappend(input, &context);
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
        INDENT; fprintf(stream, "<%s>\n", root->name);
        indent_level++;
        if (root->value != NULL){
            INDENT;
            fprintf(stream, "%s\n", root->value);
        }
        parsed += gpn_to_file(stream, root->child);
        indent_level--;
        INDENT; fprintf(stream, "</%s>\n", root->name);
        parsed++;
        root = root->next;
    }

    #undef INDENT
    return parsed;
}
