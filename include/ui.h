#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "game.h"

/* Initialise la fenêtre Raylib et la surface de rendu logique. */
bool ui_init(void);

/* Libère les ressources graphiques créées par ui_init. */
void ui_close(void);

/* Indique si l'utilisateur ou la fenêtre demande la fermeture du jeu. */
bool ui_should_close(void);

/* Traduit les interactions utilisateur en opérations sur la partie. */
bool ui_handle_input(Game *game);

/* Dessine l'état courant de la partie dans la fenêtre. */
void ui_draw(const Game *game);

#endif
