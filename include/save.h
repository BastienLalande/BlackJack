#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>

typedef struct {
    int balance;
    int wins;
    int losses;
} SaveData;

/* Charge les statistiques ; initialise les valeurs par défaut si le fichier manque. */
bool save_load(SaveData *data);

/* Valide et écrit les statistiques dans le fichier de sauvegarde. */
bool save_write(const SaveData *data);

#endif
