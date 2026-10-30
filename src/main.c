#include "game.h"
#include "save.h"
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Charge la sauvegarde, initialise le jeu et pilote la boucle principale de l'application. */
int main(void)
{
    SaveData save_data;
    /* Une sauvegarde absente est initialisée par save_load ; une erreur réelle arrête le jeu. */
    if (!save_load(&save_data)) {
        perror("Impossible de lire blackjack.sav");
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));

    /* Transfère les données persistées dans l'état de jeu utilisé par les modules. */
    Game game;
    game_init(&game, save_data.balance, save_data.wins, save_data.losses);
    if (!ui_init()) {
        fprintf(stderr, "Impossible d'initialiser la fenetre Raylib.\n");
        return EXIT_FAILURE;
    }

    /* Traite les entrées, persiste les changements signalés, puis dessine chaque image. */
    while (!ui_should_close()) {
        if (ui_handle_input(&game)) {
            /* Synchronise les seules données persistantes depuis l'état courant. */
            save_data.balance = game.balance;
            save_data.wins = game.wins;
            save_data.losses = game.losses;
            if (!save_write(&save_data)) {
                perror("Impossible d'ecrire blackjack.sav");
            }
        }

        ui_draw(&game);
    }

    /* Ferme proprement la fenêtre et libère les ressources Raylib. */
    ui_close();
    return EXIT_SUCCESS;
}