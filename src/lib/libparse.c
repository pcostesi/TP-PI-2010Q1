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
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#define LIBPARSE 0.1

/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*
 *  General includes
 */

#include "libparse.c"

/* Macros and constants */

#define BLOCSIZE 16

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
        if (tmp_p == NULL) {return NULL}
        buf = tmp_p;
    }
    buf[c_size] = c;
    c_size++;
    if (c == '\0') {
        tmp_p = realloc(buf, c_size * sizeof(char));
        buf_size = 0;
        buf = NULL;
        c_size = 0;
        return tmp_p;
    }
    return buf;
}

/* Public functions */

gpnode_p gpn_alloc(void)
{
    gpnode_p node = malloc(sizeof(gpnode_t));
    if (node == NULL){
        printf("OUT OF MEMORY! :(");
        exit(255);
    }
    return node;
}

void gpn_free(gpnode_p node){
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

/*
 * COMMENTS IN THE FOLLOWING FUNCTION ARE TO BE REMOVED IN THE NEXT
 * DEVELOPMENT ITERATION. THIS INFORMATION BELONGS TO THE DOCUMENTATION,
 * AND THE FUNCTION NEEDS SHORTER COMMENTS.
 */

gpnode_p parse(FILE *stream)
{
    gpnode_p node = NULL;
    gpnode_p last_child = NULL;
    static enum {WHITESPACE, DATA, STAG, ETAG} States;
    int input;
    States state = WHITESPACE;

    while ((input = fgetc(stream)) != EOF){
        switch(state):
            case WHITESPACE:
                if (input == '<'){
                    /* Whitespace is anything that comes after a closing
                     * tag. We assume the Start of the file is itself
                     * a root node (since the true nature of
                     * xml-like structures is an n'ary-tree). */
                    state = STAG;
                    node = gpn_alloc();
                } else if (!isspace(input) && input != 0){
                    /* Push contents into buffer -- it's the content. In
                     * case the file has an invalid opening tag, the
                     * matching test at ETAG will return NULL (which
                     * propagates to the top, resulting in an 'Invalid
                     * File Warning'). */
                    state = DATA;
                    buffer((char) input);
                }
                break;
            case DATA:
                if (input == '<'){
                    /* An opening tag indicator means the function has
                     * to call itself to parse the child node. Remember
                     * that xml-like files are trees, and trees are
                     * recursive data structures. In case an invalid
                     * node is retrieved, propagate the error and free
                     * the allocated data structures. */
                    ungetc(input, stream);
                    if (last_child == NULL){
                        last_child = parse(stream)
                        node->child = last_child;
                    } else {
                        last_child->next = parse(stream);
                        last_child = last_child->next;
                    }
                } else if (input != 0){
                    buffer(input);
                }
                break;
            case STAG:
                if (input == '<'){
                    /* the format is not quite right. Return Null. Clean
                     * up the mess. */
                    gpn_free(node);
                    free(buffer(0));
                    return NULL;
                } else if (input == '>'){
                    /* It's the end of the tag. Take the buffer, assign
                     * the pointer to node->name and change state to
                     * whitespace */
                    state = WHITESPACE;
                    node->name = buffer('\0');
                } else if (input == '/'){
                    state = ETAG;
                } else if (input != '\0') {
                    buffer(input);
                }
                break;
            case ETAG:
                if (input == '<'){
                    /* WTF? NULL it down! Don't forget to free the
                     * memory! */
                    gpn_free(node);
                    free(buffer(0));
                    return NULL;
                } else if (input == '>'){
                    /* Retrieve the pointer to the text and empty the
                     * buffer. Compare the opening tag with the last one
                     * to see if they match and return the node,
                     * otherwise return NULL (and free resources) */
                    if (strcmp(node->name, buffer(0)) == 0){
                        return node;
                    } else {
                        return NULL;
                    }
                } else if (input != 0){
                  /* As the buffer function uses 0 to reset the stack,
                   * it would be unwise to let the users add a
                   * sentinel like 0 to a file, potentially causing bugs
                   * such as data loss. Let's ignore the zero
                   * altogether, because it would confuse functions like
                   *  printf. */
                    buffer(input);
                }
                break;
    }
    return NULL;
}
