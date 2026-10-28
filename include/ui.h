#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "game.h"

bool ui_init(void);
void ui_close(void);
bool ui_should_close(void);
bool ui_handle_input(Game *game);
void ui_draw(const Game *game);

#endif
