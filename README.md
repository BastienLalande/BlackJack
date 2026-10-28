# Blackjack en C

Jeu de Blackjack graphique en C avec Raylib. Les mises sont faites par pas de
10 jetons ; les figures valent 10, l'As vaut 1 ou 11, et le croupier reste sur
17. Un Blackjack naturel est paye 3:2. Le solde et le bilan des manches sont
enregistres dans `blackjack.sav`, dans le repertoire depuis lequel le jeu est
lance.

## Dependances

- GCC ou un compilateur C compatible avec C11
- Raylib
- GNU Make

Sous Windows avec MSYS2 UCRT64, installez les outils et Raylib depuis le terminal
UCRT64 :

```sh
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-raylib
```

Sous Linux, installez les paquets de developpement de Raylib et `pkg-config`,
puis `make` (par exemple `libraylib-dev` et `pkg-config` sur Debian/Ubuntu).

## Compiler et lancer

```sh
make
./blackjack
```

Sous Windows, lancez `blackjack.exe` depuis le terminal UCRT64.

## Commandes

- Cliquez sur `-` ou `+` pour ajuster la mise.
- Cliquez sur `Distribuer`, puis `Tirer` ou `Rester` pendant votre tour.
- Cliquez sur `Manche suivante` pour continuer.
- Si le solde tombe a zero, le jeu permet de repartir avec 100 jetons.
- Appuyez sur Echap ou fermez la fenetre pour quitter.
