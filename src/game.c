#include "game.h"

#include <limits.h>

static bool draw_card(Game *game, Card *hand, size_t *count)
{
    if (*count >= HAND_CAPACITY) {
        return false;
    }

    if (!deck_draw(&game->deck, &hand[*count])) {
        return false;
    }

    ++(*count);
    return true;
}

int game_hand_value(const Card *hand, size_t count)
{
    int total = 0;
    int aces = 0;

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

    while (total > 21 && aces > 0) {
        total -= 10;
        --aces;
    }

    return total;
}

static void finish_round(Game *game, RoundResult result)
{
    game->result = result;
    game->phase = GAME_ROUND_OVER;

    switch (result) {
    case RESULT_PLAYER_WIN: {
        long long balance = (long long)game->balance + (long long)game->bet * 2;
        game->balance = balance > INT_MAX ? INT_MAX : (int)balance;
        ++game->wins;
        break;
    }
    case RESULT_BLACKJACK: {
        long long payout = (long long)game->bet + (long long)game->bet * 3 / 2;
        long long balance = (long long)game->balance + payout;
        game->balance = balance > INT_MAX ? INT_MAX : (int)balance;
        ++game->wins;
        break;
    }
    case RESULT_PUSH: {
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

static void resolve_dealer_turn(Game *game)
{
    int player_value = game_hand_value(game->player_hand, game->player_count);
    int dealer_value;

    while ((dealer_value = game_hand_value(game->dealer_hand, game->dealer_count)) < 17
           && game->dealer_count < HAND_CAPACITY) {
        if (!draw_card(game, game->dealer_hand, &game->dealer_count)) {
            break;
        }
    }

    dealer_value = game_hand_value(game->dealer_hand, game->dealer_count);
    if (dealer_value > 21 || player_value > dealer_value) {
        finish_round(game, RESULT_PLAYER_WIN);
    } else if (player_value < dealer_value) {
        finish_round(game, RESULT_DEALER_WIN);
    } else {
        finish_round(game, RESULT_PUSH);
    }
}

void game_init(Game *game, int balance, int wins, int losses)
{
    deck_init(&game->deck);
    deck_shuffle(&game->deck);
    game->player_count = 0;
    game->dealer_count = 0;
    game->balance = balance;
    game->bet = balance < 10 ? balance : 10;
    game->wins = wins;
    game->losses = losses;
    game->phase = GAME_BETTING;
    game->result = RESULT_NONE;
}

bool game_change_bet(Game *game, int amount)
{
    if (game->phase != GAME_BETTING || amount == 0 || game->balance == 0) {
        return false;
    }

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

bool game_start_round(Game *game)
{
    if (game->phase != GAME_BETTING || game->bet < 1 || game->bet > game->balance) {
        return false;
    }

    if (DECK_SIZE - game->deck.next_card < 2 * HAND_CAPACITY) {
        deck_init(&game->deck);
        deck_shuffle(&game->deck);
    }

    game->player_count = 0;
    game->dealer_count = 0;
    game->result = RESULT_NONE;
    game->balance -= game->bet;

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

    if (player_blackjack && dealer_blackjack) {
        finish_round(game, RESULT_PUSH);
    } else if (player_blackjack) {
        finish_round(game, RESULT_BLACKJACK);
    } else if (dealer_blackjack) {
        finish_round(game, RESULT_DEALER_WIN);
    }

    return true;
}

bool game_hit(Game *game)
{
    if (game->phase != GAME_PLAYER_TURN
        || !draw_card(game, game->player_hand, &game->player_count)) {
        return false;
    }

    int value = game_hand_value(game->player_hand, game->player_count);
    if (value > 21) {
        finish_round(game, RESULT_DEALER_WIN);
    } else if (value == 21) {
        resolve_dealer_turn(game);
    }

    return true;
}

bool game_stand(Game *game)
{
    if (game->phase != GAME_PLAYER_TURN) {
        return false;
    }

    resolve_dealer_turn(game);
    return true;
}
