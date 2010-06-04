/*
 *      strings.c
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

#ifndef STRUTILS
#define STRUTILS 1

/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*
 *  General includes
 */

#include "strings.h"

/*
 *  Macros and constants
 */
#define BLOCKSIZE 32

/**
 * @brief
 *
 * @param
 *
 * @return
 */
int
nlen(int in)
{
    int size = 0;
    if (in == 0)
        size = 1;
    for (; in != 0; size++, in /= 10)
        if (in < 0)
            in = in * -10;
    return size;
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
char *
int2str(int in)
{
    char * out;
    int size = nlen(in);
    out = malloc(size + 1);
    sprintf(out, "%d", in);
    return out;
}


/*
 *  Static functions
 */

/*
 *  Public functions
 */


/**
 * @brief Simple implementation of strdup.
 *
 * @param string Pointer to char array.
 *
 * @return array of chars, an identical copy of the string.
 *
 * As most implementations of the stdlib include this function as "strdup",
 * I've chosen to name it dupstr. The GNU Standard Library includes it.
 *
 */
char *
dupstr(const char * string)
{
    char * new = malloc(strlen(string) + 1);
    if (new != NULL) strcpy(new, string);
    return new;
}


/**
 * @brief Context initializer for String Utilities.
 *
 * @param str Environment
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

/**
 * @brief frees any heap-alloc'd Environment Carrier and its buffer.
 *
 * @param s String Environment
 *
 * Frees the Environment Carrier and its contents.
 */
void
strfree(string_p s)
{
    if (s != NULL)
        free(s->buffer);
    free(s);
}

/**
 * @brief heap-allocate a new String Environment Carrier.
 *
 * @return New String Environment Carrier.
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

/**
 * @brief strappend -- push a new character into the buffer.
 *
 * @param c Input character
 * @param s String Environment
 *
 * @return Appended character, or 0 in case of error.
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
            s->buffer_size = BLOCKSIZE;
            s->last_non_whitespace_idx = 0;
            s->char_idx = 0;
        } else if (s->buffer_size < s->char_idx + 2){
            tmp = realloc(s->buffer, s->buffer_size + BLOCKSIZE);
            if (tmp == NULL) return 0;
            s->buffer = tmp;
            s->buffer_size += BLOCKSIZE;
        }

        /* Set the last non-whitespace char */
        if (!isspace(c))
            s->last_non_whitespace_idx = s->char_idx + 1;
        /* Do the actual push into the string and null-terminate it */
        s->buffer[s->char_idx++] = c;
        s->buffer[s->char_idx] = 0;
    }
    return c;
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
char *
strtrm(string_p s)
{
    char * exitval = NULL;
    if (s != NULL && s->buffer != NULL && \
            s->buffer_size >= s->last_non_whitespace_idx){
        /* Trim buffer and make room to add a trailing zero (sentinel).
         * Since array indices start at zero but sizes start at one, we
         * need to add 1 to the last_non_whitespace_idx. */
        s->buffer = realloc(s->buffer, s->last_non_whitespace_idx + 1);
        s->buffer[s->last_non_whitespace_idx] = 0;
        s->char_idx = s->last_non_whitespace_idx;
        s->buffer_size = s->last_non_whitespace_idx + 1;
        exitval = s->buffer;
    }
    return exitval;
}


/**
 * @brief strpop -- detaches a null-terminated, trimmed string and
 * resets the Environment Carrier.
 *
 * @param s String Environment
 *
 * @return Pointer to array of chars.
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

#endif
