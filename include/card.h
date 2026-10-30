#ifndef CARD_H
#define CARD_H

#include <stdbool.h>
#include <stddef.h>

#define DECK_SIZE 52

/* Enseigne d'une carte, dans l'ordre utilisé pour construire le paquet. */
typedef enum {
    COEUR,
    CARREAU,
    PIQUE,
    TREFLE
} Suit;

/* Une carte est définie par son enseigne et son rang (de l'As au Roi). */
typedef struct {
    Suit suit;
    int value; // de 1 a 13 (1 AS, 13 Roi)
} Card;

/* Paquet standard et position de la prochaine carte à distribuer. */
typedef struct {
    Card cards[DECK_SIZE];
    size_t next_card;
} Deck;

/* Remplit le paquet avec les 52 combinaisons et remet le compteur au début. */
void deck_init(Deck *deck);

/* Mélange les cartes en place et réinitialise la prochaine carte à tirer. */
void deck_shuffle(Deck *deck);

/* Copie la prochaine carte dans card ; renvoie false si le paquet est épuisé. */
bool deck_draw(Deck *deck, Card *card);

#endif
