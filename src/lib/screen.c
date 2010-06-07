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
#include <string.h>
#include <stdarg.h>

/*
 *  General includes
 */

#include "screen.h"
#include "strings.h"

/*
 *  Macros and constants
 */

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

typedef struct Layer{
    char ** matrix;
    size_t x, y;
    int x_offset, y_offset;
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
static void drawMargins(screen scr, layer l);
static void zeroOut(char ** m, size_t x, size_t y);
static void freeMatrix(char ** m, size_t y);


/*
 *  Static functions
 */

static void
drawTitle(screen scr, layer l)
{
    #define _SET_BUFFER(A) if (itr_x++ >= 0 && itr_x < l->x + 1) \
                                scr->buffer[itr_y][itr_x] = (A)
    int itr_x, itr_y;
    char * t = NULL;
    int y_bound = MIN(l->y + l->y_offset, scr->y);
    int x_bound = MIN(l->x + l->x_offset, scr->x);

    /* Draw title */
    if (l->name != NULL && (itr_y = l->y_offset - 1) >= 0){
        /* place the cursor over the title, offset 1 the layer = (1; -1)*/
        itr_x = l->x_offset;
        /* move the boundary so the title doesn't stick out of the margins */
        x_bound -= 4; /* because we're adding ')', ' ', '~' and '#' */
        _SET_BUFFER('(');
        _SET_BUFFER(' ');
        for (t = l->name; *t != 0 && itr_x < x_bound; t++)
            _SET_BUFFER(*t);
        _SET_BUFFER(' ');
        _SET_BUFFER(')');
    }
    #undef _SET_BUFFER
}

static void
drawMargins(screen scr, layer l)
{
    int itr_x, itr_y;
    int y_bound = MIN(l->y + l->y_offset, scr->y);
    int x_bound = MIN(l->x + l->x_offset, scr->x);

    /* Draw top and down margins */
    for (itr_y = l->y_offset - 1; itr_y < y_bound + 1; itr_y += l->y + 1){
        for (itr_x = l->x_offset; itr_x < x_bound; itr_x++){
            if (itr_x >= 0 && itr_y >= 0){
                scr->buffer[itr_y][itr_x] = '~';
            }
        }
    }

    /* Draw left and right margins */
    for (itr_y = l->y_offset; itr_y < y_bound; itr_y++){
        for (itr_x = l->x_offset - 1; itr_x < x_bound + 1; itr_x += l->x + 1){
            if (itr_x >= 0 && itr_y >= 0){
                scr->buffer[itr_y][itr_x] = '|';
            }
        }
    }

    /* Draw corners */
    for (itr_y = l->y_offset - 1; itr_y < y_bound + 1; itr_y += l->y + 1)
        for (itr_x = l->x_offset - 1; itr_x < x_bound + 1; itr_x += l->x + 1)
            if (itr_x >= 0 && itr_y >= 0)
                scr->buffer[itr_y][itr_x] = '#';
}

static void
drawLayer(screen scr, layer l)
{
    int itr_x, itr_y;

    int y_boundary = MIN(l->y + l->y_offset, scr->y);
    int x_boundary = MIN(l->x + l->x_offset, scr->x);
    
    if (!(l->mode & SCR_HIDDEN)){
        for (itr_y = l->y_offset; itr_y < y_boundary; itr_y++){
            for (itr_x = l->x_offset; itr_x < x_boundary; itr_x++){
                if (itr_y >= 0 && itr_x >= 0 && itr_y - l->y_offset >= 0 && itr_x - l->x_offset >= 0)
                    scr->buffer[itr_y][itr_x] = \
                        l->matrix[itr_y - l->y_offset][itr_x - l->x_offset];
            }
        }

        if (l->mode & SCR_DRAW_MARGINS)
            drawMargins(scr, l);
        if (l->mode & SCR_DRAW_TITLE)
            drawTitle(scr, l);
    }
}

static void
bufferToStream(screen scr)
{
    int itr_x, itr_y;
    fputc('\n', scr->stream);
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
    int itr_y;
    char ** matrix;
    matrix = malloc(y * sizeof(char *));
    if (matrix != NULL){
        for (itr_y = 0; itr_y < y; itr_y++){
            matrix[itr_y] = malloc(sizeof(char) * x);
            memset(matrix[itr_y], ' ', x);
        }
    }
    return matrix;
}

static void
freeMatrix(char ** m, size_t y)
{
    int itr_y;
    if (m != NULL){
        for (itr_y = 0; itr_y < y; itr_y++){
            free(m[itr_y]);
        }
        free(m);
    }
}

static void
zeroOut(char ** m, size_t x, size_t y)
{
    int itr = 0;
    if (m != NULL){
        for (itr = 0; itr < y; itr++){
            memset(m[itr], ' ', x);
        }
    }
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
newLayer(size_t x, size_t y, int x_offset, int y_offset)
{
    layer result = malloc(sizeof(layer_t));
    if (result != NULL){
        result->matrix = matrixAlloc(x, y);
        result->y = y;
        result->x = x;
        result->x_offset = x_offset;
        result->y_offset = y_offset;
        result->mode = SCR_AUTO_WARP | SCR_DRAW_MARGINS | SCR_DRAW_TITLE;
        result->name = NULL;
    }
    return result;
}

void
absMoveLayer(layer l, int x, int y)
{
    if (l != NULL){
        l->x_offset = x;
        l->y_offset = y;
    }
}

layer
draw(layer l, const char ** m)
{
    int itr_x, itr_y;
    if (l != NULL){
        for (itr_y = 0; m[itr_y] != NULL && itr_y < l->y; itr_y++){
            setText(l, itr_y, m[itr_y]);
        }
    }
    return l;
}

layer
setText(layer l, int i, const char * t)
{
    int itr_x;
    for (itr_x = 0; itr_x < l->x && t[itr_x] != 0; itr_x++){
        l->matrix[i][itr_x] = t[itr_x];
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
        freeMatrix(scr->buffer, scr->y);
        free(scr);
    }
}


void
setTitle(layer l, const char * c)
{
    l->name = malloc(strlen(c) + 1);
    strcpy(l->name, c);
}

void
setMode(layer l, int i)
{
    l->mode = i; 
}

void
freeLayer(layer l)
{
    if (l != NULL){
        freeMatrix(l->matrix, l->y);
        if (l->name != NULL)
            free(l->name);
        free(l);
    }
}

char *
gauge(char * s, size_t capacity, size_t percentage)
{
    size_t itr;
    for (itr = 0; itr < capacity; itr++){
        if (itr < capacity * percentage / 100 && itr < capacity){
            s[itr] = '=';
        } else {
            s[itr] = '-';
        }
    }
}


layer
gaugeWidget(const char * name, size_t size)
{
    layer l = newLayer(size + 2, 1, 0, 0); /* leave spaces */
    setTitle(l, name);
    gaugeWidgetUpdate(l, 100); /* set it to 100% */
    return l;
}


void
gaugeWidgetUpdate(layer l, size_t percentage)
{
    gauge(&(l->matrix[0][1]), l->x - 2, percentage);
}

/* TODO: replace this with a space-aware, self-warping function. */
layer
text(layer l, const char *s)
{
    int itr;
    int x = 0, y = 0;
    zeroOut(l->matrix, l->x, l->y);
    for (itr = 0; s[itr] != 0 && y < l->y; itr++){
        if (x < l->x){
            l->matrix[y][x++] = s[itr];
        } else if (l->mode & SCR_AUTO_WARP) {
            x = 0;
            y++;
            itr--;
        }
    }
    return l;
}

layer
resizeLayer(layer l, size_t x, size_t y)
{
    int itr_y;
    char ** m = matrixAlloc(x, y);
    if (m != NULL && l != NULL){
        for (itr_y = 0; itr_y < l->y; itr_y++){
            memcpy(m[itr_y], l->matrix[itr_y], l->x);
        }
        freeMatrix(l->matrix, l->y);
        l->matrix = m;
        l->y = y;
        l->x = x;
    }
    return l;
    
}

layer
vmenu(layer l, const char ** opts)
{
    zeroOut(l->matrix, l->x, l->y);
    int itr;
    char * opt = malloc(1);
    for (itr = 0; opts[itr] != NULL; itr++);
    l = resizeLayer(l, l->x, itr + 2);
    for (itr = 0; opts[itr] != NULL && itr < l->y - 1; itr++){
        opt = realloc(opt, strlen(opts[itr]) + nlen(itr + 1) + strlen(" -   ") + 1);
        sprintf(opt, "  %d - %s", itr + 1, opts[itr]);
        setText(l, itr + 1, opt);
    }
    free(opt);
    return l;
}


#endif

