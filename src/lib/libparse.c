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
char * extract_up_to(FILE *stream, char lim){
    int len = 0, c, len2 = 0;
    char *result = NULL;
    while ((c = fgetc(stream)) != EOF && c != lim){
        if (len >= len2){
            result = realloc(result, sizeof(char) * (len + BLOCKSIZE));
            len2 = sizeof(char) * (len + BLOCKSIZE);
        }
        if (result != NULL){
            result[len] = c;
            len++;
        }
    }
    result = realloc(result, len + 1);
    result[len] = 0;
    return result;
}

/* Yes, I am insane. Thanks. */
gpnode_p parse(FILE *stream)
{
    gpnode_p root = NULL;
    gpnode_p node = NULL;
    static enum {WHITESPACE, BTAGS, STAG, ETAG} States;
    static States state = ROOT;
    int input;

    while ((input = fgetc(stream)) != EOF){
    return root;
}
