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


/* Public function prototypes */

int gpn_to_file(FILE *, gpnode_p);
int gpn_cmp_tag(gpnode_p, const char *);
int gpn_ncmp_tag(gpnode_p, const char *, int);
gpnode_p gpn_init(gpnode_p);
void gpn_free(gpnode_p);
gpnode_p gpn_alloc(void);
gpnode_p parse(FILE *, int *, int *);
gpnode_p new_gpn_child(gpnode_p);
gpnode_p gpn_next(gpnode_p);
gpnode_p gpn_prev(gpnode_p);
gpnode_p gpn_parent(gpnode_p);
gpnode_p gpn_child(gpnode_p);
void gpn_link_as_child(gpnode_p, gpnode_p);
void gpn_set_tag(gpnode_p, char *);
void gpn_set_content(gpnode_p, char *);
char * gpn_get_tag(gpnode_p);
char * gpn_get_content(gpnode_p);

#endif
