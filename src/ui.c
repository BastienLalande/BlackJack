#include "ui.h"
#include "raylib.h"
#include <stdio.h>

// Résolution logique de référence (votre jeu est conçu pour ça)
#define LOGICAL_WIDTH 1280
#define LOGICAL_HEIGHT 720

// Anciennes macros de dimensions remplacées ou adaptées
#define SCREEN_WIDTH LOGICAL_WIDTH
#define SCREEN_HEIGHT LOGICAL_HEIGHT

static const Color TABLE_GREEN_COLOR = {24, 105, 73, 255};
static const Color DARK_GREEN_COLOR = {15, 75, 53, 255};
static const Color GOLD_COLOR = {245, 196, 74, 255};
static const Color CARD_RED_COLOR = {185, 48, 55, 255};
static const Color TEXT_LIGHT_COLOR = {246, 245, 231, 255};

// RenderTexture pour le redimensionnement virtuel (plein écran propre)
static RenderTexture2D target = { 0 };

static Vector2 GetVirtualMousePosition(void) {
    Vector2 mouse = GetMousePosition();
    float scale = (float)GetScreenWidth() / LOGICAL_WIDTH;
    if ((float)GetScreenHeight() / LOGICAL_HEIGHT < scale) {
        scale = (float)GetScreenHeight() / LOGICAL_HEIGHT;
    }
    float offsetX = (GetScreenWidth() - (LOGICAL_WIDTH * scale)) * 0.5f;
    float offsetY = (GetScreenHeight() - (LOGICAL_HEIGHT * scale)) * 0.5f;

    return (Vector2){ (mouse.x - offsetX) / scale, (mouse.y - offsetY) / scale };
}

static bool point_in_button(Rectangle button)
{
    return CheckCollisionPointRec(GetVirtualMousePosition(), button);
}

static bool clicked_button(Rectangle button)
{
    return point_in_button(button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static void draw_button(Rectangle bounds, const char *label, bool enabled)
{
    bool hovered = point_in_button(bounds);
    Color background = enabled ? (hovered ? GOLD_COLOR : DARK_GREEN_COLOR) : (Color){80, 85, 82, 255};
    Color foreground = enabled && hovered ? DARK_GREEN_COLOR : TEXT_LIGHT_COLOR;

    DrawRectangleRounded(bounds, 0.18f, 8, background);
    DrawRectangleRoundedLines(bounds, 0.18f, 8, enabled ? GOLD_COLOR : GRAY);
    int text_width = MeasureText(label, 22);
    DrawText(label, (int)(bounds.x + (bounds.width - text_width) / 2),
             (int)(bounds.y + (bounds.height - 22) / 2), 22, foreground);
}

static const char *rank_name(int value)
{
    static const char *names[] = {
        "", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
    };
    return value >= 1 && value <= 13 ? names[value] : "?";
}

static const char *suit_name(Suit suit)
{
    static const char *names[] = {"C", "D", "P", "T"};
    return suit >= COEUR && suit <= TREFLE ? names[suit] : "?";
}

static void draw_card(Card card, float x, float y, bool hidden)
{
    const float width = 70;
    const float height = 100;

    if (hidden) {
        DrawRectangleRounded((Rectangle){x, y, width, height}, 0.12f, 8, DARK_GREEN_COLOR);
        DrawRectangleRoundedLines((Rectangle){x, y, width, height}, 0.12f, 8, GOLD_COLOR);
        DrawText("?", (int)x + 25, (int)y + 32, 32, GOLD_COLOR);
        return;
    }

    DrawRectangleRounded((Rectangle){x, y, width, height}, 0.12f, 8, RAYWHITE);
    DrawRectangleRoundedLines((Rectangle){x, y, width, height}, 0.12f, 8, LIGHTGRAY);
    Color ink = card.suit == COEUR || card.suit == CARREAU ? CARD_RED_COLOR : DARKGREEN;
    DrawText(rank_name(card.value), (int)x + 7, (int)y + 7, 23, ink);
    DrawText(suit_name(card.suit), (int)x + 8, (int)y + 37, 22, ink);
    DrawText(rank_name(card.value), (int)x + 48, (int)y + 70, 18, ink);
}

static void draw_hand(const Card *hand, size_t count, float y, bool hide_first)
{
    const float card_width = 70;
    const float spacing = 42;
    float total_width = count == 0 ? 0 : card_width + (float)(count - 1) * spacing;
    float x = (SCREEN_WIDTH - total_width) / 2;

    for (size_t i = 0; i < count; ++i) {
        draw_card(hand[i], x + (float)i * spacing, y, hide_first && i == 1);
    }
}

bool ui_init(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    
    // Ouvre une fenêtre par défaut en 1280x720 (ou vous pouvez passer en plein écran direct)
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Casino");
    
    // Si vous voulez forcer le plein écran dès le lancement, décommentez la ligne ci-dessous :
    // ToggleFullscreen();

    if (!IsWindowReady()) {
        return false;
    }

    // Crée une texture de rendu virtuelle pour garder le ratio et l'affichage propre
    target = LoadRenderTexture(LOGICAL_WIDTH, LOGICAL_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    SetTargetFPS(60);
    return true;
}

void ui_close(void)
{
    UnloadRenderTexture(target);
    CloseWindow();
}

bool ui_should_close(void)
{
    return WindowShouldClose();
}

bool ui_handle_input(Game *game)
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        return false;
    }

    // Touche F11 pour basculer en Plein Écran / Fenêtré à tout moment
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }

    if (game->phase == GAME_BETTING) {
        if (clicked_button((Rectangle){322, 556, 58, 46})) {
            return game_change_bet(game, -10);
        }
        if (clicked_button((Rectangle){580, 556, 58, 46})) {
            return game_change_bet(game, 10);
        }
        if (clicked_button((Rectangle){390, 615, 180, 52})) {
            return game_start_round(game);
        }
    } else if (game->phase == GAME_PLAYER_TURN) {
        if (clicked_button((Rectangle){340, 615, 130, 52})) {
            return game_hit(game);
        }
        if (clicked_button((Rectangle){490, 615, 130, 52})) {
            return game_stand(game);
        }
    } else if (game->phase == GAME_ROUND_OVER
                && clicked_button((Rectangle){350, 615, 260, 52})) {
        if (game->balance == 0) {
            game->balance = 100;
            game->bet = 10;
        }
        game->phase = GAME_BETTING;
        game->result = RESULT_NONE;
        game->player_count = 0;
        game->dealer_count = 0;
        return true;
    }

    return false;
}

void ui_draw(const Game *game)
{
    // 1. On dessine dans la texture virtuelle (résolution fixe propre)
    BeginTextureMode(target);
    ClearBackground(TABLE_GREEN_COLOR);
    
    DrawRectangle(0, 0, SCREEN_WIDTH, 90, DARK_GREEN_COLOR);
    DrawText("CASINO DE BABA", 36, 24, 34, GOLD_COLOR);

    char status[128];
    snprintf(status, sizeof(status), "Jetons : %d    Victoires : %d    Defaites : %d",
             game->balance, game->wins, game->losses);
    DrawText(status, 405, 35, 18, TEXT_LIGHT_COLOR);
    DrawText("Croupier", 50, 124, 22, TEXT_LIGHT_COLOR);
    DrawText("Joueur", 50, 370, 22, TEXT_LIGHT_COLOR);
    draw_hand(game->dealer_hand, game->dealer_count, 155,
              game->phase == GAME_PLAYER_TURN);
    draw_hand(game->player_hand, game->player_count, 405, false);

    if (game->dealer_count > 0 && game->phase != GAME_PLAYER_TURN) {
        snprintf(status, sizeof(status), "Total du croupier : %d",
                 game_hand_value(game->dealer_hand, game->dealer_count));
        DrawText(status, 720, 175, 16, TEXT_LIGHT_COLOR);
    }
    if (game->player_count > 0) {
        snprintf(status, sizeof(status), "Votre total : %d",
                 game_hand_value(game->player_hand, game->player_count));
        DrawText(status, 720, 426, 16, TEXT_LIGHT_COLOR);
    }

    DrawLine(80, 535, 880, 535, (Color){154, 195, 164, 255});

    if (game->phase == GAME_BETTING) {
        snprintf(status, sizeof(status), "Mise : %d jetons", game->bet);
        DrawText(status, 392, 552, 22, TEXT_LIGHT_COLOR);
        draw_button((Rectangle){322, 556, 58, 46}, "-", game->balance > 0);
        draw_button((Rectangle){580, 556, 58, 46}, "+",
                    game->balance > game->bet);
        draw_button((Rectangle){390, 615, 180, 52}, "Distribuer",
                    game->bet > 0 && game->bet <= game->balance);
    } else if (game->phase == GAME_PLAYER_TURN) {
        snprintf(status, sizeof(status), "Mise en jeu : %d jetons", game->bet);
        DrawText(status, 410, 555, 20, TEXT_LIGHT_COLOR);
        draw_button((Rectangle){340, 615, 130, 52}, "Tirer", true);
        draw_button((Rectangle){490, 615, 130, 52}, "Rester", true);
    } else {
        const char *message = "Manche terminee";
        switch (game->result) {
        case RESULT_PLAYER_WIN: message = "Vous gagnez !"; break;
        case RESULT_DEALER_WIN: message = "Le croupier gagne."; break;
        case RESULT_PUSH: message = "Egalite : mise rendue."; break;
        case RESULT_BLACKJACK: message = "Blackjack ! Paiement 3:2."; break;
        case RESULT_NONE: break;
        }
        DrawText(message, 375, 555, 23, GOLD_COLOR);
        draw_button((Rectangle){350, 615, 260, 52},
                    game->balance == 0 ? "Recevoir 100 jetons" : "Manche suivante",
                    true);
    }

    DrawText("Valeurs : figures = 10, As = 1 ou 11. Le croupier reste a 17. [F11] Plein ecran",
             200, 690, 14, TEXT_LIGHT_COLOR);
             
    EndTextureMode();

    // 2. On affiche la texture étirée et centrée sur l'écran réel (fenêtré ou plein écran)
    BeginDrawing();
    ClearBackground(BLACK); // Couleur des bandes noires si le ratio diffère

    float scale = (float)GetScreenWidth() / LOGICAL_WIDTH;
    float screenHeightRatio = (float)GetScreenHeight() / LOGICAL_HEIGHT;
    if (screenHeightRatio < scale) {
        scale = screenHeightRatio;
    }

    float destWidth = LOGICAL_WIDTH * scale;
    float destHeight = LOGICAL_HEIGHT * scale;
    float destX = (GetScreenWidth() - destWidth) * 0.5f;
    float destY = (GetScreenHeight() - destHeight) * 0.5f;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
    Rectangle destRec = { destX, destY, destWidth, destHeight };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(target.texture, sourceRec, destRec, origin, 0.0f, WHITE);

    EndDrawing();
}