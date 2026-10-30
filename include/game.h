#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stddef.h>

#include "card.h"

#define HAND_CAPACITY 12

/* Étapes possibles d'une manche, de la mise à son résultat. */
typedef enum {
    GAME_BETTING,
    GAME_PLAYER_TURN,
    GAME_ROUND_OVER
} GamePhase;

/* Issue calculée lors de la fin d'une manche. */
typedef enum {
    RESULT_NONE,
    RESULT_PLAYER_WIN,
    RESULT_DEALER_WIN,
    RESULT_PUSH,
    RESULT_BLACKJACK
} RoundResult;

/* État complet de la partie, incluant le paquet, les mains et les statistiques. */
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

/* Initialise une partie avec le solde et les statistiques persistés. */
void game_init(Game *game, int balance, int wins, int losses);

/* Ajuste la mise pendant la phase de pari, sans dépasser le solde ni descendre sous 1. */
bool game_change_bet(Game *game, int amount);

/* Débite la mise, distribue les deux mains et traite immédiatement les blackjacks. */
bool game_start_round(Game *game);

/* Tire une carte pour le joueur et termine ou poursuit la manche selon le total. */
bool game_hit(Game *game);

/* Termine le tour du joueur et fait jouer le croupier jusqu'à au moins 17. */
bool game_stand(Game *game);

/* Calcule la valeur d'une main en comptant chaque As à 11 ou à 1 selon le total. */
int game_hand_value(const Card *hand, size_t count);

#endif
