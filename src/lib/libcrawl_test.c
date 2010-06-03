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


/*
 *  System includes
 */

#include <stdio.h>
#include <stdlib.h>

/*
 *  General includes
 */

#include "libcrawl.h"
#include "state.h"

int
main(int argc, char** argv)
{
    logbook book;
    character_t walter;
    game_t * g;
    game_t * game = load_game("test.xml");
    printf("Game is %p\n", (void *) game);
    book = Logbook(14, &walter, "log.xml");
    book = logmsg(book, "Test");
    book = logmsg(book, "Test2");
    book = logmsg(book, "Test3");
    book = logmsg(book, "Test4");
    book = logmsg(book, "Test5");

    walter.name = "Walter";
    walter.HP = 42;
    walter.professionID = 1;
    walter.roomID = 3;
    if (game == NULL){
            printf("Invalid game file.\n");
    } else {
            printf( "Data about game:\n"
                "You enter through %d and exit through %d\n"
                "\tProfessions:\t\t%d\n"
                "\tEnemies:\t\t%d\n"
                "\tRooms in game:\t\t%d\n",
                game->StartRoomID, game->ExitRoomID, game->professions_size,
                            game->enemies_size, game->rooms_size);
            save_state(game, book, "state.xml");
            g = load_state("state.xml", book);
            printf( "Data about SAVED game:\n"
                "You enter through %d and exit through %d\n"
                "\tProfessions:\t\t%d\n"
                "\tEnemies:\t\t%d\n"
                "\tRooms in game:\t\t%d\n",
                game->StartRoomID, game->ExitRoomID, game->professions_size,
                            game->enemies_size, game->rooms_size);
            free_game(g);
            free_game(game);
    }

    log_to_disk(book, "log.xml");
    free_logbook(book);

    return EXIT_SUCCESS;
}

/*
 *  Static functions
 */

/*
 *  Public functions
 */
