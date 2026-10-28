#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stddef.h>

#include "card.h"

#define HAND_CAPACITY 12

typedef enum {
    GAME_BETTING,
    GAME_PLAYER_TURN,
    GAME_ROUND_OVER
} GamePhase;

typedef enum {
    RESULT_NONE,
    RESULT_PLAYER_WIN,
    RESULT_DEALER_WIN,
    RESULT_PUSH,
    RESULT_BLACKJACK
} RoundResult;

typedef struct {
    Deck deck;
    Card player_hand[HAND_CAPACITY];
    Card dealer_hand[HAND_CAPACITY];
    size_t player_count;
    size_t dealer_count;
    int balance;
    int bet;
    int wins;
    int losses;
    GamePhase phase;
    RoundResult result;
} Game;

void game_init(Game *game, int balance, int wins, int losses);
bool game_change_bet(Game *game, int amount);
bool game_start_round(Game *game);
bool game_hit(Game *game);
bool game_stand(Game *game);
int game_hand_value(const Card *hand, size_t count);

#endif
