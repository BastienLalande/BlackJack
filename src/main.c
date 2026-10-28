#include "game.h"
#include "save.h"
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    SaveData save_data;
    if (!save_load(&save_data)) {
        perror("Impossible de lire blackjack.sav");
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));

    Game game;
    game_init(&game, save_data.balance, save_data.wins, save_data.losses);
    if (!ui_init()) {
        fprintf(stderr, "Impossible d'initialiser la fenetre Raylib.\n");
        return EXIT_FAILURE;
    }

    while (!ui_should_close()) {
        if (ui_handle_input(&game)) {
            save_data.balance = game.balance;
            save_data.wins = game.wins;
            save_data.losses = game.losses;
            if (!save_write(&save_data)) {
                perror("Impossible d'ecrire blackjack.sav");
            }
        }

        ui_draw(&game);
    }

    ui_close();
    return EXIT_SUCCESS;
}