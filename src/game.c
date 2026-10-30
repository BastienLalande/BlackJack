#include "game.h"

#include <limits.h>

/* Ajoute une carte à une main si sa capacité et le paquet le permettent. */
static bool draw_card(Game *game, Card *hand, size_t *count)
{
    /* La main ne doit jamais dépasser la taille de son tableau fixe. */
    if (*count >= HAND_CAPACITY) {
        return false;
    }

    if (!deck_draw(&game->deck, &hand[*count])) {
        return false;
    }

    /* Le nombre de cartes n'augmente qu'après un tirage réussi. */
    ++(*count);
    return true;
}

/* Additionne les valeurs des cartes et réduit les As de 11 à 1 si nécessaire. */
int game_hand_value(const Card *hand, size_t count)
{
    int total = 0;
    int aces = 0;

    /* Compte d'abord les As comme 11 et les figures comme 10. */
    for (size_t i = 0; i < count; ++i) {
        if (hand[i].value == 1) {
            total += 11;
            ++aces;
        } else if (hand[i].value >= 10) {
            total += 10;
        } else {
            total += hand[i].value;
        }
    }

    /* Chaque réduction d'un As enlève 10 points, jusqu'à revenir à 21 ou moins. */
    while (total > 21 && aces > 0) {
        total -= 10;
        --aces;
    }

    return total;
}

/* Enregistre l'issue de la manche, met à jour le solde et les statistiques. */
static void finish_round(Game *game, RoundResult result)
{
    game->result = result;
    game->phase = GAME_ROUND_OVER;

    /* La mise a déjà été retirée au début de la manche : les gains incluent ici
       le retour de la mise lorsque cela s'applique. */
    switch (result) {
    case RESULT_PLAYER_WIN: {
        /* Une victoire standard rend la mise initiale et verse un gain égal. */
        long long balance = (long long)game->balance + (long long)game->bet * 2;
        game->balance = balance > INT_MAX ? INT_MAX : (int)balance;
        ++game->wins;
        break;
    }
    case RESULT_BLACKJACK: {
        /* Le blackjack paie 3:2, en plus de restituer la mise engagée. */
        long long payout = (long long)game->bet + (long long)game->bet * 3 / 2;
        long long balance = (long long)game->balance + payout;
        game->balance = balance > INT_MAX ? INT_MAX : (int)balance;
        ++game->wins;
        break;
    }
    case RESULT_PUSH: {
        /* En cas d'égalité, seule la mise engagée est rendue. */
        long long balance = (long long)game->balance + game->bet;
        game->balance = balance > INT_MAX ? INT_MAX : (int)balance;
        break;
    }
    case RESULT_DEALER_WIN:
    case RESULT_NONE:
        ++game->losses;
        break;
    }
}

/* Joue automatiquement le tour du croupier, puis compare les deux mains. */
static void resolve_dealer_turn(Game *game)
{
    int player_value = game_hand_value(game->player_hand, game->player_count);
    int dealer_value;

    /* Le croupier tire tant que son total est inférieur à 17 et que la main a de la place. */
    while ((dealer_value = game_hand_value(game->dealer_hand, game->dealer_count)) < 17
           && game->dealer_count < HAND_CAPACITY) {
        if (!draw_card(game, game->dealer_hand, &game->dealer_count)) {
            break;
        }
    }

    /* Un dépassement de 21 par le croupier ou un total joueur supérieur donne la victoire. */
    dealer_value = game_hand_value(game->dealer_hand, game->dealer_count);
    if (dealer_value > 21 || player_value > dealer_value) {
        finish_round(game, RESULT_PLAYER_WIN);
    } else if (player_value < dealer_value) {
        finish_round(game, RESULT_DEALER_WIN);
    } else {
        finish_round(game, RESULT_PUSH);
    }
}

/* Prépare une nouvelle partie et initialise son paquet et son état de pari. */
void game_init(Game *game, int balance, int wins, int losses)
{
    deck_init(&game->deck);
    deck_shuffle(&game->deck);
    game->player_count = 0;
    game->dealer_count = 0;
    /* La mise initiale est plafonnée à 10 et au solde actuellement disponible. */
    game->balance = balance;
    game->bet = balance < 10 ? balance : 10;
    game->wins = wins;
    game->losses = losses;
    game->phase = GAME_BETTING;
    game->result = RESULT_NONE;
}

/* Modifie la mise uniquement pendant les paris et la garde dans les limites valides. */
bool game_change_bet(Game *game, int amount)
{
    if (game->phase != GAME_BETTING || amount == 0 || game->balance == 0) {
        return false;
    }

    /* Les additions sont bornées par le solde ; les diminutions ne passent pas sous 1.
       Le cas INT_MIN évite un débordement lors du calcul de sa valeur absolue. */
    if (amount > 0) {
        if (game->bet > game->balance - amount) {
            game->bet = game->balance;
        } else {
            game->bet += amount;
        }
    } else if (amount == INT_MIN || amount <= 1 - game->bet) {
        game->bet = 1;
    } else {
        game->bet += amount;
    }

    return true;
}

/* Débute une manche : vérifie la mise, distribue les cartes et détecte les blackjacks. */
bool game_start_round(Game *game)
{
    if (game->phase != GAME_BETTING || game->bet < 1 || game->bet > game->balance) {
        return false;
    }

    /* Renouvelle le paquet avant la manche si le nombre de cartes restantes est faible. */
    if (DECK_SIZE - game->deck.next_card < 2 * HAND_CAPACITY) {
        deck_init(&game->deck);
        deck_shuffle(&game->deck);
    }

    game->player_count = 0;
    game->dealer_count = 0;
    game->result = RESULT_NONE;
    /* La mise est placée avant la distribution et sera restituée selon le résultat. */
    game->balance -= game->bet;

    /* Distribue alternativement deux cartes au joueur et au croupier. */
    if (!draw_card(game, game->player_hand, &game->player_count)
        || !draw_card(game, game->dealer_hand, &game->dealer_count)
        || !draw_card(game, game->player_hand, &game->player_count)
        || !draw_card(game, game->dealer_hand, &game->dealer_count)) {
        game->balance += game->bet;
        game->phase = GAME_BETTING;
        return false;
    }

    game->phase = GAME_PLAYER_TURN;

    int player_value = game_hand_value(game->player_hand, game->player_count);
    int dealer_value = game_hand_value(game->dealer_hand, game->dealer_count);
    bool player_blackjack = player_value == 21;
    bool dealer_blackjack = dealer_value == 21;

    /* Le blackjack est traité immédiatement : égalité, paiement joueur ou victoire croupier. */
    if (player_blackjack && dealer_blackjack) {
        finish_round(game, RESULT_PUSH);
    } else if (player_blackjack) {
        finish_round(game, RESULT_BLACKJACK);
    } else if (dealer_blackjack) {
        finish_round(game, RESULT_DEALER_WIN);
    }

    return true;
}

/* Fait tirer une carte au joueur et résout immédiatement un dépassement ou un 21. */
bool game_hit(Game *game)
{
    if (game->phase != GAME_PLAYER_TURN
        || !draw_card(game, game->player_hand, &game->player_count)) {
        return false;
    }

    int value = game_hand_value(game->player_hand, game->player_count);
    /* Un total supérieur à 21 perd ; à 21, le tour du croupier est automatiquement joué. */
    if (value > 21) {
        finish_round(game, RESULT_DEALER_WIN);
    } else if (value == 21) {
        resolve_dealer_turn(game);
    }

    return true;
}

/* Termine volontairement le tour du joueur et délègue le résultat au tour du croupier. */
bool game_stand(Game *game)
{
    if (game->phase != GAME_PLAYER_TURN) {
        return false;
    }

    resolve_dealer_turn(game);
    return true;
}
