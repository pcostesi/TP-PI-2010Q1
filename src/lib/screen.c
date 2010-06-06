/*
 *      screen.c
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

#ifndef _LIB_SCR
#define _LIB_SCR 1
/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *  General includes
 */

#include "screen.h"

/*
 *  Macros and constants
 */


typedef struct Layer{
    char ** matrix;
    size_t x, y;
    size_t x_offset, y_offset;
    int mode;
    char * name;
} layer_t;


typedef struct Screen{
    char ** buffer;
    size_t x, y;
    FILE * stream;
} screen_t;


/*
 *  Static function prototypes
 */

static char ** matrixAlloc(size_t, size_t);
static void drawLayer(screen scr, layer l);
static void bufferToStream(screen scr);

/*
 *  Static functions
 */

static void drawMargins(screen scr, layer l)
{
    int itr_x, itr_y;
    char c;
}

static void drawLayer(screen scr, layer l)
{
    int itr_x, itr_y;
    for (itr_y = l->y_offset; \
         itr_y < scr->y && itr_y < l->y + l->y_offset; \
         itr_y++){
        for (itr_x = l->x_offset; \
             itr_x < scr->x && itr_x < l->x + l->x_offset; \
             itr_x++){
            scr->buffer[itr_y][itr_x] = \
                l->matrix[itr_y - l->y_offset][itr_x - l->x_offset];
        }
    }
}

static void bufferToStream(screen scr)
{
    int itr_x, itr_y;
    for (itr_y = 0; itr_y < scr->y; itr_y++){
        for (itr_x = 0; itr_x < scr->x; itr_x++){
            fputc(scr->buffer[itr_y][itr_x], scr->stream);
        }
        if (itr_y != scr->y - 1) fputc('\n', scr->stream);
    }
    fflush(scr->stream);
}

static char **
matrixAlloc(size_t x, size_t y)
{
    int itr_y, itr_x;
    char ** matrix;
    matrix = malloc(y * sizeof(char *));
    if (matrix != NULL){
        for (itr_y = 0; itr_y < y; itr_y++){
            matrix[itr_y] = malloc(sizeof(char) * x);
            for (itr_x = 0; itr_x < x; itr_x++){
                matrix[itr_y][itr_x] = ' ';
            }
        }
    }
    return matrix;
}

/*
 *  Public functions
 */

screen
initscr(FILE * stream, size_t x, size_t y)
{
    int itr_y, itr_x;
    char ** buffer;

    screen scr = malloc(sizeof(screen_t));
    if (scr != NULL){
        scr->stream = stream;
        scr->buffer = matrixAlloc(x, y);
        scr->x = x;
        scr->y = y;
    }
    return scr;
}

layer
newLayer(size_t x, size_t y, size_t x_offset, size_t y_offset)
{
    layer result = malloc(sizeof(layer_t));
    if (result != NULL){
        result->matrix = matrixAlloc(x, y);
        result->y = y;
        result->x = x;
        result->x_offset = x_offset;
        result->y_offset = y_offset;
        result->mode = SCR_AUTO_WARP | SCR_DRAW_MARGINS;
        result->name = NULL;
    }
    return result;
}

void
absMoveLayer(layer l, size_t x, size_t y)
{
    if (l != NULL){
        l->x_offset = x;
        l->y_offset = y;
    }
}

layer
draw(layer l, const char ** m)
{
    int x, y, itr_x, itr_y;
    if (l != NULL){
        y = l->y;
        x = l->x;
        for (itr_y = 0; m[itr_y] != NULL && itr_y < l->y; itr_y++){
            for (itr_x = 0; itr_y < l->x && m[itr_y][itr_x] != 0; itr_x++){
                l->matrix[itr_y][itr_x] = m[itr_y][itr_x];
            }
        }
    }
    return l;
}

void
clrscr(screen scr)
{
    int itr_y, itr_x;
    /* clear the screen */
    for (itr_y = 0; itr_y < scr->y; itr_y++)
        for (itr_x = 0; itr_x < scr->x; itr_x++)
            scr->buffer[itr_y][itr_x] = ' ';
}

void
update(screen scr, layer * layers)
{
    int itr_y, itr_x, itr_layers;

    clrscr(scr);
    if (scr != NULL){
        /* draw each layer, sequentially */
        for (itr_layers = 0; layers[itr_layers] != NULL; itr_layers++){
            drawLayer(scr, layers[itr_layers]);
        }
        bufferToStream(scr);
    }
}


void
endscr(screen scr)
{
    if (scr != NULL){
        fputc('\n', scr->stream);
        free(scr->buffer);
        free(scr);
    }
}

void
freeLayer(layer l)
{
    if (l != NULL){
        free(l->matrix);
        free(l);
    }
}

#endif
