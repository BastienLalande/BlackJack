#include "save.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define SAVE_FILE "blackjack.sav"
#define SAVE_MAGIC "BLACKJACK_SAVE"
#define SAVE_VERSION 1
#define DEFAULT_BALANCE 1000

bool save_load(SaveData *data)
{
    if (data == NULL) {
        errno = EINVAL;
        return false;
    }

    FILE *file = fopen(SAVE_FILE, "r");
    if (file == NULL) {
        if (errno == ENOENT) {
            data->balance = DEFAULT_BALANCE;
            data->wins = 0;
            data->losses = 0;
            return true;
        }
        return false;
    }

    char magic[32];
    int version;
    int trailing;
    bool valid = fscanf(file, "%31s %d", magic, &version) == 2
        && fscanf(file, "%d %d %d", &data->balance, &data->wins, &data->losses) == 3
        && fscanf(file, "%d", &trailing) == EOF
        && ferror(file) == 0;

    if (fclose(file) != 0) {
        return false;
    }

    if (!valid || strcmp(magic, SAVE_MAGIC) != 0 || version != SAVE_VERSION
        || data->balance < 0 || data->wins < 0 || data->losses < 0) {
        errno = EINVAL;
        return false;
    }

    return true;
}

bool save_write(const SaveData *data)
{
    if (data == NULL || data->balance < 0 || data->wins < 0 || data->losses < 0) {
        errno = EINVAL;
        return false;
    }

    FILE *file = fopen(SAVE_FILE, "w");
    if (file == NULL) {
        return false;
    }

    bool success = fprintf(file, "%s %d\n%d %d %d\n",
                           SAVE_MAGIC, SAVE_VERSION,
                           data->balance, data->wins, data->losses) >= 0;
    if (fclose(file) != 0) {
        success = false;
    }
    return success;
}
