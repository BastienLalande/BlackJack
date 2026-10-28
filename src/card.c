#include "card.h"

#include <stdlib.h>

void deck_init(Deck *deck)
{
    size_t index = 0;

    for (int suit = COEUR; suit <= TREFLE; ++suit) {
        for (int value = 1; value <= 13; ++value) {
            deck->cards[index].suit = (Suit)suit;
            deck->cards[index].value = value;
            ++index;
        }
    }

    deck->next_card = 0;
}

void deck_shuffle(Deck *deck)
{
    for (size_t i = DECK_SIZE - 1; i > 0; --i) {
        size_t j = (size_t)rand() % (i + 1);
        Card temporary = deck->cards[i];

        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temporary;
    }

    deck->next_card = 0;
}

bool deck_draw(Deck *deck, Card *card)
{
    if (deck->next_card >= DECK_SIZE) {
        return false;
    }

    *card = deck->cards[deck->next_card];
    ++deck->next_card;
    return true;
}
