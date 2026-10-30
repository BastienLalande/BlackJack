#include "card.h"

#include <stdlib.h>

/* Construit un paquet standard non mélangé et remet l'index de tirage à zéro. */
void deck_init(Deck *deck)
{
    size_t index = 0;

    /* Chaque enseigne reçoit les rangs de l'As au Roi. */
    for (int suit = COEUR; suit <= TREFLE; ++suit) {
        for (int value = 1; value <= 13; ++value) {
            deck->cards[index].suit = (Suit)suit;
            deck->cards[index].value = value;
            ++index;
        }
    }

    deck->next_card = 0;
}

/* Applique le mélange de Fisher-Yates afin de permuter le paquet sur place. */
void deck_shuffle(Deck *deck)
{
    for (size_t i = DECK_SIZE - 1; i > 0; --i) {
        /* Choisit une position parmi les cartes encore à mélanger et échange les cartes. */
        size_t j = (size_t)rand() % (i + 1);
        Card temporary = deck->cards[i];

        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temporary;
    }

    deck->next_card = 0;
}

/* Tire la carte située à next_card, puis avance l'index si une carte existe. */
bool deck_draw(Deck *deck, Card *card)
{
    /* Ne pas lire au-delà du tableau lorsque toutes les cartes ont été distribuées. */
    if (deck->next_card >= DECK_SIZE) {
        return false;
    }

    *card = deck->cards[deck->next_card];
    ++deck->next_card;
    return true;
}
