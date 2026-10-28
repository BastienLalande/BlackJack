#ifndef CARD_H
#define CARD_H

#include <stdbool.h>
#include <stddef.h>

#define DECK_SIZE 52

typedef enum {
    COEUR,
    CARREAU,
    PIQUE,
    TREFLE
} Suit;

typedef struct {
    Suit suit;
    int value;
} Card;

typedef struct {
    Card cards[DECK_SIZE];
    size_t next_card;
} Deck;

void deck_init(Deck *deck);
void deck_shuffle(Deck *deck);
bool deck_draw(Deck *deck, Card *card);

#endif
