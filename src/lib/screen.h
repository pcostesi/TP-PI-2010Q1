/*
 *      screen.h
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

#ifndef __LIB_SCREEN
#define __LIB_SCREEN 1

/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *  General includes
 */

/*
 *  Macros and constants
 */

typedef struct Screen * screen;
typedef struct Layer * layer;

#define SCR_DRAW_MARGINS 2
#define SCR_DRAW_TITLE 8
#define SCR_AUTO_WRAP 1
#define SCR_HIDDEN 4
#define SCR_NO_AUTO_RESIZE 16
#define STD_HEIGHT 25
#define STD_WIDTH 80
#define ENV_HEIGHT (getenv("LINES") == NULL ? STD_HEIGHT : atoi(getenv("LINES")))
#define ENV_WIDTH (getenv("COLUMNS") == NULL ? STD_WIDTH : atoi(getenv("COLUMNS")))

/*
 *  Public function prototypes
 */


screen initscr(FILE *, size_t, size_t);
layer newLayer(size_t, size_t, int, int);

void setTitle(layer, const char * c);
void setMode(layer, int);
layer draw(layer, const char **);
void update(screen, layer *);
void absMoveLayer(layer, int, int);
layer setText(layer, int, int, const char *);
layer setNumber(layer l,int x, int y, unsigned int i);

void endscr(screen);
void freeLayer(layer);

layer resizeLayer(layer, size_t, size_t);

char * gauge(char *, size_t, size_t);
layer gaugeWidget(const char *, size_t);
void gaugeWidgetUpdate(layer, size_t);

layer vmenu(layer, const char *, const char **);

layer idxtext(layer, int, const char *);
layer text(layer, const char *);

int getScreenDimensions(screen, int *, int *);
void centerText(layer, const char *);
#endif
