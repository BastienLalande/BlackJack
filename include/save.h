#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>

typedef struct {
    int balance;
    int wins;
    int losses;
} SaveData;

bool save_load(SaveData *data);
bool save_write(const SaveData *data);

#endif
