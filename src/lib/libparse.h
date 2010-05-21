/*
 *      libparse.h
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

#ifndef __LIB_PARSE
#define __LIB_PARSE 1

#include <stdio.h>


/* GPNode -- General Purpose Node for representation of tree-like data. */
typedef struct GPNode * gpnode_p;

typedef struct GPNode{
    gpnode_p next, prev;
    gpnode_p child, parent;
    char *value;
    char *name;
} gpnode_t;


/*
 * String -- Convinience 'Environment Carrier' for str* functions in
 * libparse.
 *
 * THOU SHALT NOT ACCESS THE INTERNALS OF THIS STRUCT. Failure to attain
 * to this commandment, doom and dismay may be cast upon thine soul. Ye
 * hath been warn'd. */
typedef struct String * string_p;

typedef struct String{
    char * buffer;
    size_t buffer_size;
    size_t last_non_whitespace_idx;
    size_t char_idx;
} string_t;

/* Public function prototypes */

gpnode_p   parse(FILE *, int *, int *);
int         gpn_to_file(FILE *, gpnode_p);
gpnode_p   gpn_alloc(void);
void        gpn_init(gpnode_p);
void        gpn_free(gpnode_p);
gpnode_p   child(gpnode_p);


void strinit(string_p);
string_p strnew(void);
int strappend(char, string_p);
char * strtrm(string_p);
char * strpop(string_p);
void strfree(string_p);


#endif
