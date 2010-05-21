/*
 *      filename.c
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


#define PROJECT_NAME_VERSION 0.1

/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *  General includes
 */

#include "libparse.h"

/*
 *  Public function prototypes
 */

void gpn_print(gpnode_p);

int
main(int argc, char** argv)
{

    FILE *fp;
    int line, col, parsed;
    gpnode_p root;
    char * filename = NULL;
    if (argc == 1){
        printf("Write the name of the file you want to parse:\n");
        filename = malloc(33);
        scanf("%32s", filename);

    } else {
        filename = argv[1];
    }
    printf("Your file: %-32s\n", filename);
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("File doesn't exist!\n");
        free(filename);
        return 2;
    }
    root = parse(fp, &line, &col);
    if (root == NULL){
        printf("Parsing error at line %d, column %d!\n", line, col);
        fclose(fp);
        return 1;
    }
    printf("parseado\n");
    parsed = gpn_to_file(stdout, root);
    printf("\nParsed: %d nodes.\n", parsed);
    gpn_free(root);
    fclose(fp);

    char * s = NULL;
    /*string_p str_context = strnew();*/
    string_t str_;
    string_p str_context = &str_;
    strinit(str_context);
    strappend('h', str_context);
    strappend('o', str_context);
    strappend('l', str_context);
    strappend('a', str_context);
    strappend(' ', str_context);
    strappend('\n', str_context);
    strappend(' ', str_context);
    strappend('h', str_context);
    strappend('o', str_context);
    strappend('l', str_context);
    strappend('a', str_context);
    strappend(' ', str_context);
    strappend('\n', str_context);
    strappend(' ', str_context);
    s = strpop(str_context);
    printf("TEST STRING:\n*%s*\n", s);
    free(s);
    strappend(' ', str_context);
    s = strpop(str_context);
    printf("TEST STRING:\n*%s*\n", s);
    free(s);
    strappend('A', str_context);
    s = strpop(str_context);
    printf("TEST STRING:\n*%s*\n", s);
    free(s);
    return EXIT_SUCCESS;
}


/*
 *  Public functions
 */
