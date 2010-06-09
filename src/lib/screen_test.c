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
#include "screen.h"
/*
 *  Macros and constants
 */

/*
 *  Static function prototypes
 */

/*
 *  Public function prototypes
 */

int
main(int argc, char** argv)
{
    screen scr = initscr(stdout, ENV_WIDTH, ENV_HEIGHT);
    layer l = newLayer(42, 3, 4, 4);
    layer l2 = newLayer(20, 3, 15, 3);
    layer l3 = gaugeWidget("Health", 35);
    const char * m[] = {
        "Esto es una prueba",
        "para ver si anda",
         NULL
        };
    draw(l, m);
    draw(l2, m);
    vmenu(l2, "Esto es un menu bastante largo que deberia autoacomodarse", (const char *[]){"this", "is", "a", "test", NULL});
    layer layers[] = {l, l2, l3, NULL};
    absMoveLayer(l3, 1, 1);
    update(scr, layers);
    absMoveLayer(l, 2, 2);
    gaugeWidgetUpdate(l3, 60);
    setTitle(l, "Layer 1");
    vmenu(l2, "Esto es un menu bastante largo que deberia autoacomodarse", (const char *[]){"test", "over", NULL});
    update(scr, layers);
    absMoveLayer(l2, 20, 2);
    update(scr, layers);
    setMode(l, SCR_HIDDEN);
    text(l2, "Esto es otra prueba distinta para ver si se la banca la func.");
    gaugeWidgetUpdate(l3, 20);
    update(scr, layers);
    endscr(scr);
    freeLayer(l);
    freeLayer(l2);
    freeLayer(l3);
    printf("Ready.\n");
    return EXIT_SUCCESS;
}

/*
 *  Static functions
 */

/*
 *  Public functions
 */
