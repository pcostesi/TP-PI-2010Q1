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

void print(gpnode_p);

int
main(int argc, char** argv)
{
    printf("asdasd\n");
    FILE *fp;
    fp = fopen("test.xml", "r");
    printf("El archivo apunta a %p\n", fp);
    gpnode_p root = parse(fp);
    printf("root apunta a %p\n", root);
    printf("child apunta a %p\n", root->child);
    print(root);
    fclose(fp);
    return EXIT_SUCCESS;
}


/*
 *  Public functions
 */

void
print(gpnode_p root)
{
    static indent = 0;
    while (root != NULL){
        int x;
        for (x = 0; x < indent; x++) printf("\t");
        printf("<%s>\n", (char *)(root->name));
        if (((char *) root->value) != NULL)
        for (x = 0; x < indent; x++) printf("\t");
        if ((char *)(root->value) != NULL)
            printf("\t%s\n", (char *)(root->value));
        indent++;
        print(root->child);
        indent--;
        for (x = 0; x < indent; x++) printf("\t");
        printf("</%s>\n", (char *)(root->name));
        root = root->next;
    }

}
