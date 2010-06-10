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
#include <ctype.h>

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
    const char * name;
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


/**
 * @brief
 *
 * @param
 *
 * @return
 */
static void
drawTitle(screen scr, layer l)
{
    #define _SET_BUFFER(A) if (itr_x++ >= 0 && itr_x < x_bound) \
                                scr->buffer[itr_y][itr_x] = (A)
    int itr_x, itr_y;
    const char * t = NULL;
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

/**
 * @brief
 *
 * @param
 *
 * @return
 */
static void
drawMargins(screen scr, layer l)
{
    int itr_x, itr_y;
    int y_bound = MIN(l->y + l->y_offset, scr->y);
    int x_bound = MIN(l->x + l->x_offset, scr->x);

    /* Draw top and down margins */
    for (itr_y = l->y_offset - 1; itr_y < y_bound + 1; itr_y += l->y + 1){
        for (itr_x = l->x_offset; itr_x < x_bound; itr_x++){
            if (itr_x >= 0 && itr_y >= 0 && itr_x < scr->x && itr_y < scr->y){
                scr->buffer[itr_y][itr_x] = '-';
            }
        }
    }

    /* Draw left and right margins */
    for (itr_y = l->y_offset; itr_y < y_bound; itr_y++){
        for (itr_x = l->x_offset - 1; itr_x < x_bound + 1; itr_x += l->x + 1){
            if (itr_x >= 0 && itr_y >= 0 && itr_x < scr->x && itr_y < scr->y){
                scr->buffer[itr_y][itr_x] = '|';
            }
        }
    }

    /* Draw corners */
    for (itr_y = l->y_offset - 1; itr_y < y_bound + 1; itr_y += l->y + 1)
        for (itr_x = l->x_offset - 1; itr_x < x_bound + 1; itr_x += l->x + 1)
            if (itr_x >= 0 && itr_y >= 0 && itr_x < scr->x && itr_y < scr->y)
                scr->buffer[itr_y][itr_x] = '#';
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
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

/**
 * @brief
 *
 * @param
 *
 * @return
 */
static void
bufferToStream(screen scr)
{
    int itr_x, itr_y;
    fputc('\n', scr->stream);
    for (itr_y = 0; itr_y < scr->y; itr_y++){
        for (itr_x = 0; itr_x < scr->x; itr_x++){
            /* Avoid problematic chars in non-ASCII encodings */
            if (128 > (unsigned char) scr->buffer[itr_y][itr_x])
                fputc(scr->buffer[itr_y][itr_x], scr->stream);
            else
                fputc(' ', scr->stream);
        }
        if (itr_y != scr->y - 1) fputc('\n', scr->stream);
    }
    fflush(scr->stream);
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
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

/**
 * @brief
 *
 * @param
 *
 * @return
 */
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

/**
 * @brief
 *
 * @param
 *
 * @return
 */
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

/**
 * @brief
 *
 * @param
 *
 * @return
 */
screen
initscr(FILE * stream, size_t x, size_t y)
{
    screen scr = malloc(sizeof(screen_t));
    if (scr != NULL){
        scr->stream = stream;
        scr->buffer = matrixAlloc(x, y);
        scr->x = x;
        scr->y = y;
    }
    return scr;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
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
        result->mode = SCR_NORMAL;
        result->name = NULL;
    }
    return result;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
absMoveLayer(layer l, int x, int y)
{
    if (l != NULL){
        l->x_offset = x;
        l->y_offset = y;
    }
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
layer
draw(layer l, const char ** m)
{
    int itr_y;
    if (l != NULL){
        for (itr_y = 0; m[itr_y] != NULL && itr_y < l->y; itr_y++){
            setText(l, 0, itr_y, m[itr_y]);
        }
    }
    return l;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
layer
setText(layer l,int x, int y, const char * t)
{
    int itr_x;
    if (l->y <= y || l->x <= x) return NULL;
    for (itr_x = x; itr_x < l->x && t[itr_x - x] != 0; itr_x++){
        l->matrix[y][itr_x] = t[itr_x - x];
    }
    return l;
}

layer
setNumber(layer l,int x, int y, unsigned int i)
{
    int itr_x;
    if (l->y <= y || l->x <= x) return NULL;
    for (itr_x = x; itr_x < l->x && i > 0; itr_x++, i /= 10){
        l->matrix[y][itr_x] = i % 10 + '0';
    }
    return l;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
clrscr(screen scr)
{
    int itr_y, itr_x;
    /* clear the screen */
    for (itr_y = 0; itr_y < scr->y; itr_y++)
        for (itr_x = 0; itr_x < scr->x; itr_x++)
            scr->buffer[itr_y][itr_x] = ' ';
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
update(screen scr, layer * layers)
{
    int itr_layers;
    clrscr(scr);
    if (scr != NULL){
        /* draw each layer, sequentially */
        for (itr_layers = 0; layers[itr_layers] != NULL; itr_layers++){
            drawLayer(scr, layers[itr_layers]);
        }
        bufferToStream(scr);
    }
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
endscr(screen scr)
{
    if (scr != NULL){
        fputc('\n', scr->stream);
        freeMatrix(scr->buffer, scr->y);
        free(scr);
    }
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
setTitle(layer l, const char * c)
{
    /*
    l->name = malloc(strlen(c) + 1);
    strcpy(l->name, c);
    */
    l->name = c;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
setMode(layer l, int i)
{
    l->mode = i;
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
freeLayer(layer l)
{
    if (l != NULL){
        freeMatrix(l->matrix, l->y);
        free(l);
    }
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
char *
gauge(char * s, size_t capacity, size_t percentage)
{
    size_t itr;
    size_t idx2;
    int insertIdx;
    if (capacity < 5)
		return NULL;
    idx2 = (capacity - 4) / 2;
    if (percentage > 100){
		insertIdx = sprintf(&(s[idx2]), "%d", (int) (percentage / 100));
		percentage %= 101;
	} else {
		insertIdx = sprintf(&(s[idx2]), "%d%%", (int) percentage);
	}
    for (itr = 0; itr < capacity; itr++){
        if (itr < insertIdx + idx2 && itr >= idx2) {
        } else if (itr < capacity * percentage / 100 && itr < capacity){
            s[itr] = '=';
        } else {
            s[itr] = '-';
        }
    }
    return s;
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
layer
gaugeWidget(const char * name, size_t size)
{
    layer l = newLayer(size + 2, 1, 0, 0); /* leave spaces */
    setTitle(l, name);
    gaugeWidgetUpdate(l, 100); /* set it to 100% */
    return l;
}


/**
 * @brief
 *
 * @param
 *
 * @return
 */
void
gaugeWidgetUpdate(layer l, size_t percentage)
{
    gauge(&(l->matrix[0][1]), l->x - 2, percentage);
}

layer
text (layer l, const char *s)
{
    return idxtext(l, 1, s);
}

int
is_valid_char(char c)
{
    int ret = 0;
    /* Skip problematic characters */
    if (!iscntrl(c) && c != '\n' && c != '\t' && (unsigned) c < 128)
        ret = 1;
    return ret;
}

/* TODO: replace this with a space-aware, self-WRAPing function. */
layer
idxtext(layer l, int start, const char *s)
{
    #define LEFT_MARGIN 1
    #define RIGHT_MARGIN 1
    int itr, aux;
    int x = LEFT_MARGIN, y = start;
    zeroOut(l->matrix, l->x, l->y);
    for (itr = 0; s[itr] != 0 && y < l->y; itr++){
        if (is_valid_char(s[itr])){
            if (x < l->x - RIGHT_MARGIN){
                l->matrix[y][x++] = s[itr];
            } else if (l->mode & SCR_AUTO_WRAP) {
                x = LEFT_MARGIN;
                y++;
                itr--;
            }
        } else if (s[itr] == '\n'){
            x = LEFT_MARGIN;
            y++;
        } else if (s[itr] == '\t'){
            aux = 4;
            while (aux--){
                if (x < l->x){
                    l->matrix[y][x++] = ' ';
                } else aux = 0;
            }
        }
    }
    return l;
    #undef LEFT_MARGIN
    #undef RIGHT_MARGIN
}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
layer
resizeLayer(layer l, size_t x, size_t y)
{
    int itr_y;
    char ** m = matrixAlloc(x, y);
    if (m != NULL && l != NULL){
        for (itr_y = 0; itr_y < MIN(l->y, y); itr_y++){
            memcpy(m[itr_y], l->matrix[itr_y], MIN(l->x, x));
        }
        freeMatrix(l->matrix, l->y);
        l->matrix = m;
        l->y = y;
        l->x = x;
    }
    return l;

}

/**
 * @brief
 *
 * @param
 *
 * @return
 */
layer
vmenu(layer l, const char * txt, const char ** opts)
{
    int itr, x;
    /* get the text height + 2 lines*/
    int normalTextSize = strlen(txt) / l->x + 3;
    zeroOut(l->matrix, l->x, l->y);
    /* Get the number of options to show. */
    for (itr = 0; opts[itr] != NULL; itr++);
    if (SCR_NO_AUTO_RESIZE != (l->mode & SCR_NO_AUTO_RESIZE))
        l = resizeLayer(l, l->x, itr + normalTextSize + 1);
    idxtext(l, 1, txt);
    for (itr = 0; opts[itr] != NULL && itr + normalTextSize < l->y - 1; itr++){
        x = nlen(itr + 1) + 3;
        /* snprintf is not ANSI C (std89), it's ISO C (std99). Any
         * half-decent implementation has it. Anyway... */
        setNumber(l, 1, normalTextSize + itr, (unsigned int)(itr + 1));
        setText(l, x - 2, normalTextSize + itr, ")");
        setText(l, x, normalTextSize + itr, opts[itr]);
    }
    return l;
}

int
getScreenDimensions(screen scr, int *x, int *y)
{
    int value = 1;
    if (scr != NULL){
        value = 0;
        if (x != NULL) *x = scr->x;
        if (y != NULL) *y = scr->y;
    }
    return value;
}

void
centerText(layer l, const char * t)
{
    char * t2 = dupstr(t);
    char * tok;
    int insertFrom = 0, margin = 0, lines = 1, itr = 0;
    while (t2[itr] != 0)
        if (t2[itr++] == '\n')
            lines++;
    insertFrom = (l->y - lines) / 2;
    zeroOut(l->matrix, l->x, l->y);
    for (tok = strtok(t2, "\n"); tok != NULL; tok = strtok(NULL, "\n")){
        margin = (l->x - strlen(tok)) / 2;
        setText(l, margin, insertFrom++, tok);
    }
    free(t2);
}

#endif

