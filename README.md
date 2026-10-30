# Blackjack en C

Jeu de Blackjack en C avec une interface graphique créée avec
[Raylib](https://www.raylib.com/). Le projet sépare les cartes, les règles du
jeu, l'interface et la sauvegarde en modules distincts.

## Sommaire

- [Fonctionnalités](#fonctionnalités)
- [Règles implémentées](#règles-implémentées)
- [Organisation du projet](#organisation-du-projet)
- [Dépendances](#dépendances)
- [Compilation et lancement](#compilation-et-lancement)
- [Commandes en jeu](#commandes-en-jeu)
- [Déroulement d'une manche](#déroulement-dune-manche)
- [Sauvegarde](#sauvegarde)
- [Détails des modules](#détails-des-modules)

## Fonctionnalités

- Interface graphique de 1280 × 720 pixels logiques, redimensionnable et
  affichée à l'échelle de la fenêtre.
- Paquet standard de 52 cartes, mélangé au début du jeu et renouvelé lorsqu'il
  ne reste pas suffisamment de cartes pour une manche.
- Mise réglable par pas de 1 ou 10 jetons, avec une commande « All In »,
  plafonnée au solde disponible.
- Affichage des cartes, des totaux, du solde et du bilan des manches.
- Carte cachée du croupier pendant le tour du joueur.
- Mode plein écran activable avec F11.
- Enregistrement du solde et des victoires/défaites dans `blackjack.sav`.
- Attribution de 100 jetons lorsque le joueur n'a plus de solde et démarre une
  nouvelle manche.

## Règles implémentées

- Les cartes numérotées valent leur valeur. Valet, dame et roi valent 10.
- Un as vaut 11, sauf si cela fait dépasser 21 : sa valeur est alors ramenée à 1.
  S'il y a plusieurs as, cette correction est appliquée autant de fois que
  nécessaire.
- Le joueur et le croupier reçoivent chacun deux cartes. La deuxième carte du
  croupier reste cachée pendant le tour du joueur.
- Le joueur peut tirer des cartes ou rester. Un total supérieur à 21 fait
  perdre la manche immédiatement.
- Le croupier tire tant que son total est inférieur à 17 et reste à partir de
  17.
- Lorsque les deux premières cartes du joueur totalisent 21, il s'agit d'un
  Blackjack naturel. Il paie 3:2 (la mise est rendue en plus d'un gain égal à
  une fois et demie la mise). Si le joueur et le croupier ont tous deux un
  Blackjack naturel, la manche est une égalité et la mise est rendue.
- À la comparaison finale, le total le plus élevé inférieur ou égal à 21 gagne.
  Une égalité rend la mise au joueur.

Le projet implémente un jeu simplifié : il ne propose pas de séparation de
paires, de doublement de mise, d'assurance ou de choix de règles du casino.

## Organisation du projet

```text
blackjack/
├── Makefile
├── README.md
├── include/
│   ├── card.h
│   ├── game.h
│   ├── save.h
│   └── ui.h
└── src/
    ├── card.c
    ├── game.c
    ├── main.c
    ├── save.c
    └── ui.c
```

Les fichiers `.h` déclarent les types et fonctions accessibles aux autres
modules. Les fichiers `.c` contiennent leur implémentation.

## Dépendances

- Un compilateur C compatible C11, par exemple GCC.
- Raylib et ses bibliothèques de développement.
- GNU Make.
- `pkg-config` est utilisé par le Makefile lorsqu'il est disponible pour
  récupérer les options de compilation et d'édition de liens de Raylib.

### Windows avec MSYS2 UCRT64

Ouvrez le terminal **MSYS2 UCRT64**, puis installez les paquets nécessaires :

```sh
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-raylib
```

Depuis le terminal UCRT64, placez-vous dans le dossier du projet puis compilez.
Par exemple, adaptez le chemin suivant à l'emplacement où se trouve votre
dossier :

```sh
cd "/c/chemin/vers/BlackJack"
make
```

Lancez le jeu depuis ce même terminal :

```sh
./blackjack.exe
```

Utilisez le terminal **UCRT64** et non le terminal MSYS standard : il configure
le compilateur et les bibliothèques de la bonne architecture.

### Linux

Installez GCC, GNU Make, Raylib et, si nécessaire, `pkg-config` avec le
gestionnaire de paquets de votre distribution. Par exemple, sur Debian ou
Ubuntu :

```sh
sudo apt install build-essential libraylib-dev pkg-config
```

Compilez et lancez depuis le dossier du projet :

```sh
make
./blackjack
```

## Compilation et lancement

La cible par défaut compile l'exécutable `blackjack` (ou `blackjack.exe` sous
Windows) :

```sh
make
```

Pour supprimer l'exécutable et les fichiers objets avant une nouvelle
compilation :

```sh
make clean
```

Puis lancez le fichier exécutable depuis un terminal ouvert dans le dossier du
projet. La sauvegarde est créée dans le répertoire courant au lancement.

Le Makefile accepte les variables habituelles de Make, notamment `CC`, `CFLAGS`,
`CPPFLAGS`, `LDFLAGS` et `LDLIBS`. Il demande à `pkg-config` les options Raylib
si `pkg-config` trouve le paquet `raylib`. Sinon, il utilise les bibliothèques
de liaison usuelles de la plateforme ; Raylib doit malgré tout être installée
et accessible au compilateur.

## Commandes en jeu

Les actions de jeu se font avec la souris :

1. Pendant la phase de mise, cliquez sur **-10** ou **+10** pour ajuster la mise
   de 10 jetons, sur **-1** ou **+1** pour l'ajuster d'un jeton, ou sur **All
   In** pour miser tout le solde disponible. La mise minimum est 1 jeton et la
   mise maximum est votre solde.
2. Cliquez sur **Distribuer** pour commencer la manche.
3. Pendant votre tour, cliquez sur **Tirer** pour recevoir une carte ou sur
   **Rester** pour laisser jouer le croupier.
4. Après le résultat, cliquez sur **Manche suivante** pour revenir à l'écran
   des mises.
5. Si le solde est nul, le bouton de manche suivante rétablit le solde à
   100 jetons avant de revenir aux mises.

Appuyez sur **F11** pour basculer en plein écran. Appuyez sur **Échap** ou
fermez la fenêtre pour quitter. La fenêtre peut être redimensionnée ; l'aire de
jeu conserve son format logique et est centrée dans la fenêtre.

## Déroulement d'une manche

Le jeu suit trois phases :

1. **Mise (`GAME_BETTING`)** : le joueur ajuste sa mise. Une manche ne commence
   que si la mise est supérieure ou égale à 1 et ne dépasse pas le solde.
2. **Tour du joueur (`GAME_PLAYER_TURN`)** : une carte est distribuée au joueur
   et au croupier, puis une deuxième à chacun. Le joueur tire ou reste. Un
   total de 21 obtenu après un tirage termine également son tour.
3. **Manche terminée (`GAME_ROUND_OVER`)** : le résultat et les cartes du
   croupier sont affichés. La prochaine action ramène à la phase de mise.

La mise est retirée du solde au début de la manche. En cas de victoire
ordinaire, le joueur reçoit deux fois sa mise (mise initiale incluse). Pour un
Blackjack naturel, il reçoit la mise plus son gain de 3:2. En cas d'égalité, il
récupère sa mise ; en cas de défaite, la mise est perdue. Les compteurs de
victoires et de défaites ne comptent pas les égalités.

## Sauvegarde

La progression est stockée dans `blackjack.sav`, dans le répertoire depuis
lequel le programme est lancé. Le fichier contient une signature, une version
de format, puis le solde et le nombre de victoires et de défaites. Exemple :

```text
BLACKJACK_SAVE 1
1000 0 0
```

Si le fichier n'existe pas, une nouvelle partie commence avec 1 000 jetons et
aucune victoire ni défaite. Un fichier mal formé, d'une version inconnue ou
contenant des valeurs négatives entraîne un message d'erreur au démarrage ; il
faut alors corriger ou supprimer ce fichier pour démarrer une nouvelle partie.

La sauvegarde est réécrite après chaque action qui modifie l'état du jeu
(modification de mise, distribution, tirage, résultat ou passage à la manche
suivante). Une erreur de lecture ou d'écriture est signalée dans le terminal.

## Détails des modules

### `include/card.h` et `src/card.c` — cartes et paquet

- `Suit` représente les quatre couleurs : `COEUR`, `CARREAU`, `PIQUE` et
  `TREFLE`.
- `Card` contient une couleur et une valeur de 1 à 13 : 1 représente l'as, 11
  le valet, 12 la dame et 13 le roi.
- `Deck` contient 52 cartes et l'indice de la prochaine carte à distribuer.
- `deck_init` construit le paquet complet dans l'ordre des couleurs et des
  valeurs.
- `deck_shuffle` mélange le paquet et remet l'indice de distribution à zéro.
  Le générateur pseudo-aléatoire est initialisé une fois par `main` avec
  l'heure système.
- `deck_draw` copie la carte suivante vers le paramètre de sortie et renvoie
  `false` si le paquet est épuisé.

### `include/game.h` et `src/game.c` — règles et état du jeu

- `Game` regroupe le paquet, les mains du joueur et du croupier, leurs nombres
  de cartes, le solde, la mise, les compteurs et l'état de la manche.
- Chaque main peut contenir jusqu'à `HAND_CAPACITY` cartes (12).
- `GamePhase` décrit les phases de mise, de tour du joueur et de fin de manche.
- `RoundResult` décrit les résultats possibles : victoire, défaite, égalité ou
  Blackjack.
- `game_init` initialise une nouvelle partie à partir du solde et des
  statistiques chargés, et prépare un paquet mélangé.
- `game_change_bet` modifie la mise uniquement pendant la phase de mise. Elle
  ne descend pas sous 1 et ne dépasse pas le solde.
- `game_start_round` vérifie la mise, renouvelle le paquet s'il reste moins de
  24 cartes, retire la mise, distribue les cartes de départ et traite les
  Blackjacks naturels.
- `game_hit` tire une carte pour le joueur et termine la manche en cas de
  dépassement de 21. Si le joueur atteint 21, le tour du croupier est résolu.
- `game_stand` termine le tour du joueur et résout le tour du croupier.
- `game_hand_value` calcule le total d'une main en ajustant la valeur des as
  pour éviter de dépasser 21 autant que possible.

Les fonctions d'action renvoient un booléen : `true` indique que l'action a été
acceptée et que l'état a pu changer ; `false` indique qu'elle n'est pas valide
dans la phase actuelle ou qu'une carte ne peut pas être distribuée.

### `include/ui.h` et `src/ui.c` — interface Raylib

- `ui_init` configure et ouvre la fenêtre Raylib ; elle renvoie `false` si
  l'initialisation échoue. Elle prépare également une texture de rendu de
  1280 × 720 pixels.
- `ui_should_close` indique si la fenêtre doit se fermer.
- `ui_handle_input` lit les clics et la touche Échap, puis appelle les
  fonctions du jeu appropriées. F11 bascule le plein écran.
- `ui_draw` affiche la table, les cartes, les totaux, le solde, les boutons et
  le résultat de la manche. La deuxième carte du croupier est masquée pendant
  le tour du joueur. Le rendu est effectué dans une texture logique, puis
  centré et mis à l'échelle à la taille de la fenêtre.
- `ui_close` décharge la texture de rendu et ferme la fenêtre Raylib.

Ce module est le seul à dépendre directement de Raylib pour le rendu et les
événements d'interface.

### `include/save.h` et `src/save.c` — sauvegarde

- `SaveData` regroupe les trois données persistantes : solde, victoires et
  défaites.
- `save_load` lit et valide le fichier. Si le fichier est absent, elle fournit
  les valeurs initiales d'une nouvelle partie.
- `save_write` vérifie les valeurs puis écrit le fichier de sauvegarde.
- Les deux fonctions renvoient `false` lorsqu'une erreur empêche l'opération.

### `src/main.c` — point d'entrée

`main` charge la sauvegarde, initialise le générateur pseudo-aléatoire, crée
l'état du jeu, puis ouvre l'interface. La boucle principale traite les entrées
et redessine la fenêtre. Lorsqu'une entrée change l'état, `main` actualise la
sauvegarde. À la fermeture de la fenêtre, il ferme Raylib et termine le
programme.

### `Makefile` — compilation

Le Makefile compile séparément les cinq fichiers source, puis les lie avec
Raylib pour produire l'exécutable. Les fichiers objets (`src/*.o`) sont
réutilisés tant que leurs sources n'ont pas changé. `make clean` les supprime
avec l'exécutable.

