#include "ui.h"
#include "raylib.h"
#include <stdio.h>

/* Résolution de référence indépendante de la taille réelle de la fenêtre. */
#define LOGICAL_WIDTH 1280
#define LOGICAL_HEIGHT 720
#define RENDER_SCALE 2
#define WINDOW_ICON_SIZE 64

#define SCREEN_WIDTH LOGICAL_WIDTH
#define SCREEN_HEIGHT LOGICAL_HEIGHT

static const Color TABLE_GREEN_COLOR = {24, 105, 73, 255};
static const Color DARK_GREEN_COLOR = {15, 75, 53, 255};
static const Color GOLD_COLOR = {245, 196, 74, 255};
static const Color CARD_RED_COLOR = {185, 48, 55, 255};
static const Color TEXT_LIGHT_COLOR = {246, 245, 231, 255};

/* Surface rendue en haute résolution puis réduite à la taille logique à l'écran. */
static RenderTexture2D target = { 0 };

/* Convertit les coordonnées de souris réelles en coordonnées de la surface logique,
   en tenant compte de la mise à l'échelle et des bandes de centrage. */
static Vector2 GetVirtualMousePosition(void) {
    Vector2 mouse = GetMousePosition();
    /* Choisit l'échelle uniforme la plus grande qui tient dans la fenêtre. */
    float scale = (float)GetScreenWidth() / LOGICAL_WIDTH;
    if ((float)GetScreenHeight() / LOGICAL_HEIGHT < scale) {
        scale = (float)GetScreenHeight() / LOGICAL_HEIGHT;
    }
    float offsetX = (GetScreenWidth() - (LOGICAL_WIDTH * scale)) * 0.5f;
    float offsetY = (GetScreenHeight() - (LOGICAL_HEIGHT * scale)) * 0.5f;

    /* Retire le décalage d'affichage puis inverse la mise à l'échelle. */
    return (Vector2){ (mouse.x - offsetX) / scale, (mouse.y - offsetY) / scale };
}

/* Vérifie si la souris logique est à l'intérieur du rectangle d'un bouton. */
static bool point_in_button(Rectangle button)
{
    return CheckCollisionPointRec(GetVirtualMousePosition(), button);
}

/* Signale un clic gauche sur un bouton uniquement si le curseur le survole. */
static bool clicked_button(Rectangle button)
{
    return point_in_button(button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/* Dessine un bouton avec un état visuel adapté au survol et à sa disponibilité. */
static void draw_button(Rectangle bounds, const char *label, bool enabled)
{
    bool hovered = point_in_button(bounds);
    /* Les boutons désactivés sont grisés ; les boutons actifs réagissent au survol. */
    Color background = enabled ? (hovered ? GOLD_COLOR : DARK_GREEN_COLOR) : (Color){80, 85, 82, 255};
    Color foreground = enabled && hovered ? DARK_GREEN_COLOR : TEXT_LIGHT_COLOR;

    DrawRectangleRounded(bounds, 0.18f, 8, background);
    DrawRectangleRoundedLines(bounds, 0.18f, 8, enabled ? GOLD_COLOR : GRAY);
    int text_width = MeasureText(label, 20);
    DrawText(label, (int)(bounds.x + (bounds.width - text_width) / 2),
             (int)(bounds.y + (bounds.height - 20) / 2), 20, foreground);
}

/* Retourne l'abréviation affichée pour un rang, ou '?' pour une valeur invalide. */
static const char *rank_name(int value)
{
    static const char *names[] = {
        "", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
    };
    return value >= 1 && value <= 13 ? names[value] : "?";
}

/* Retourne l'abréviation affichée pour une enseigne, ou '?' si elle est invalide. */
static const char *suit_name(Suit suit)
{
    static const char *names[] = {"C", "D", "P", "T"};
    return suit >= COEUR && suit <= TREFLE ? names[suit] : "?";
}

/* Dessine une carte visible ou son dos lorsqu'elle doit rester cachée. */
static void draw_card(Card card, float x, float y, bool hidden)
{
    const float width = 70;
    const float height = 100;

    if (hidden) {
        /* Le dos ne révèle ni le rang ni l'enseigne de la carte. */
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

/* Centre une main horizontalement et dessine ses cartes avec un espacement constant. */
static void draw_hand(const Card *hand, size_t count, float y, bool hide_first)
{
    const float card_width = 70;
    const float spacing = 42;
    float total_width = count == 0 ? 0 : card_width + (float)(count - 1) * spacing;
    float x = (SCREEN_WIDTH - total_width) / 2;

    /* La seconde carte (index 1) du croupier est masquée pendant le tour du joueur. */
    for (size_t i = 0; i < count; ++i) {
        draw_card(hand[i], x + (float)i * spacing, y, hide_first && i == 1);
    }
}

/* Configure Raylib, ouvre la fenêtre et crée la texture de rendu logique. */
bool ui_init(void)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Casino");

    if (!IsWindowReady()) {
        return false;
    }

    /* Crée une icône de carte en mémoire afin de ne pas dépendre d'un fichier externe. */
    Image icon = GenImageColor(WINDOW_ICON_SIZE, WINDOW_ICON_SIZE, DARK_GREEN_COLOR);
    if (icon.data != NULL) {
        /* Dessine une carte blanche bordée d'or et un losange rouge au centre. */
        ImageDrawRectangle(&icon, 12, 5, 40, 54, GOLD_COLOR);
        ImageDrawRectangle(&icon, 15, 8, 34, 48, RAYWHITE);
        ImageDrawTriangle(&icon, (Vector2){32, 18}, (Vector2){23, 31},
                          (Vector2){32, 44}, CARD_RED_COLOR);
        ImageDrawTriangle(&icon, (Vector2){32, 18}, (Vector2){41, 31},
                          (Vector2){32, 44}, CARD_RED_COLOR);
        SetWindowIcon(icon);
        UnloadImage(icon);
    } else {
        TraceLog(LOG_WARNING, "Impossible de generer l'icone de la fenetre.");
    }

    /* Le suréchantillonnage améliore la netteté des textes et des contours au redimensionnement. */
    target = LoadRenderTexture(LOGICAL_WIDTH * RENDER_SCALE, LOGICAL_HEIGHT * RENDER_SCALE);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    SetTargetFPS(60);
    return true;
}

/* Libère la texture de rendu et ferme la fenêtre Raylib. */
void ui_close(void)
{
    UnloadRenderTexture(target);
    CloseWindow();
}

/* Demande à Raylib si la fermeture de la fenêtre a été demandée. */
bool ui_should_close(void)
{
    return WindowShouldClose();
}

/* Traduit les touches et clics en actions métier ; true indique un état à sauvegarder. */
bool ui_handle_input(Game *game)
{
    /* Échap laisse la fermeture à la boucle principale ; F11 bascule le plein écran. */
    if (IsKeyPressed(KEY_ESCAPE)) {
        return false;
    }

    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }

    if (game->phase == GAME_BETTING) {
        /* Les boutons modifient la mise ; seule une action métier réussie est signalée. */
        if (clicked_button((Rectangle){ 470, 560, 65, 45 })) {
            return game_change_bet(game, -10);
        }
        if (clicked_button((Rectangle){ 545, 560, 45, 45 })) {
            return game_change_bet(game, -1);
        }
        if (clicked_button((Rectangle){ 600, 560, 80, 45 })) {
            /* Le montant ajouté fait passer la mise courante au solde total. */
            return game_change_bet(game, game->balance - game->bet);
        }
        if (clicked_button((Rectangle){ 690, 560, 45, 45 })) {
            return game_change_bet(game, 1);
        }
        if (clicked_button((Rectangle){ 745, 560, 65, 45 })) {
            return game_change_bet(game, 10);
        }
        if (clicked_button((Rectangle){ 540, 620, 200, 50 })) {
            return game_start_round(game);
        }
    } else if (game->phase == GAME_PLAYER_TURN) {
        if (clicked_button((Rectangle){ 490, 615, 140, 50 })) {
            return game_hit(game);
        }
        if (clicked_button((Rectangle){ 650, 615, 140, 50 })) {
            return game_stand(game);
        }
    } else if (game->phase == GAME_ROUND_OVER
                && clicked_button((Rectangle){ 500, 615, 280, 50 })) {
        /* Après une banqueroute, le bouton de manche suivante accorde un nouveau départ. */
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

/* Dessine le tapis, les mains, les informations et les commandes selon la phase de jeu. */
void ui_draw(const Game *game)
{
    /* Toutes les coordonnées et tous les éléments de jeu sont dessinés en taille logique. */
    BeginTextureMode(target);
    Camera2D render_camera = { 0 };
    render_camera.zoom = RENDER_SCALE;
    BeginMode2D(render_camera);
    ClearBackground(TABLE_GREEN_COLOR);
    
    DrawRectangle(0, 0, SCREEN_WIDTH, 90, DARK_GREEN_COLOR);
    DrawText("C CASINO", 36, 24, 34, GOLD_COLOR);

    char status[128];
    snprintf(status, sizeof(status), "Jetons : %d    Victoires : %d    Defaites : %d",
             game->balance, game->wins, game->losses);
    DrawText(status, 405, 35, 18, TEXT_LIGHT_COLOR);
    DrawText("Croupier", 50, 124, 22, TEXT_LIGHT_COLOR);
    DrawText("Joueur", 50, 370, 22, TEXT_LIGHT_COLOR);
    
    draw_hand(game->dealer_hand, game->dealer_count, 155,
              game->phase == GAME_PLAYER_TURN);
    draw_hand(game->player_hand, game->player_count, 405, false);

    /* Le total du croupier reste caché tant que le joueur peut encore agir. */
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

    DrawLine(150, 520, 1130, 520, (Color){154, 195, 164, 255});

    if (game->phase == GAME_BETTING) {
        // Affichage de la mise courante centrée
        snprintf(status, sizeof(status), "Mise : %d jetons", game->bet);
        int text_width = MeasureText(status, 20);
        DrawText(status, (LOGICAL_WIDTH - text_width) / 2, 532, 20, TEXT_LIGHT_COLOR);

        // Boutons de modification de mise (-10, -1, All In, +1, +10)
        draw_button((Rectangle){ 470, 560, 65, 45 }, "-10", game->bet >= 10);
        draw_button((Rectangle){ 545, 560, 45, 45 }, "-1", game->bet >= 1);
        draw_button((Rectangle){ 600, 560, 80, 45 }, "All In", game->balance > 0 && game->bet < game->balance);
        draw_button((Rectangle){ 690, 560, 45, 45 }, "+1", game->balance > game->bet);
        draw_button((Rectangle){ 745, 560, 65, 45 }, "+10", game->balance >= game->bet + 10);

        // Bouton Distribuer
        draw_button((Rectangle){ 540, 620, 200, 50 }, "Distribuer",
                    game->bet > 0 && game->bet <= game->balance);

    } else if (game->phase == GAME_PLAYER_TURN) {
        snprintf(status, sizeof(status), "Mise en jeu : %d jetons", game->bet);
        int text_width = MeasureText(status, 20);
        DrawText(status, (LOGICAL_WIDTH - text_width) / 2, 565, 20, TEXT_LIGHT_COLOR);

        draw_button((Rectangle){ 490, 615, 140, 50 }, "Tirer", true);
        draw_button((Rectangle){ 650, 615, 140, 50 }, "Rester", true);
    } else {
        /* Affiche le texte correspondant à l'issue enregistrée par le moteur de jeu. */
        const char *message = "Manche terminee";
        switch (game->result) {
        case RESULT_PLAYER_WIN: message = "Vous gagnez !"; break;
        case RESULT_DEALER_WIN: message = "Le croupier gagne."; break;
        case RESULT_PUSH: message = "Egalite : mise rendue."; break;
        case RESULT_BLACKJACK: message = "Blackjack ! Paiement 3:2."; break;
        case RESULT_NONE: break;
        }
        int msg_width = MeasureText(message, 23);
        DrawText(message, (LOGICAL_WIDTH - msg_width) / 2, 560, 23, GOLD_COLOR);

        draw_button((Rectangle){ 500, 615, 280, 50 },
                    game->balance == 0 ? "Recevoir 100 jetons" : "Manche suivante",
                    true);
    }

    const char *footer = "Valeurs : figures = 10, As = 1 ou 11. Le croupier reste a 17. [F11] Plein ecran";
    int footer_width = MeasureText(footer, 14);
    DrawText(footer, (LOGICAL_WIDTH - footer_width) / 2, 690, 14, TEXT_LIGHT_COLOR);
             
    EndMode2D();
    EndTextureMode();

    /* Affiche la texture logique centrée, avec une échelle uniforme sur l'écran réel. */
    BeginDrawing();
    ClearBackground(BLACK);

    float scale = (float)GetScreenWidth() / LOGICAL_WIDTH;
    float screenHeightRatio = (float)GetScreenHeight() / LOGICAL_HEIGHT;
    /* Conserve les proportions en limitant l'échelle à la dimension la plus contraignante. */
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

    /* Présente l'image complète à l'écran. */
    EndDrawing();
}