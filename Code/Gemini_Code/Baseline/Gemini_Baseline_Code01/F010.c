// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int val_count;
} KeyList;

int main() {
    KeyList *index = NULL;
    int key_count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char k[256], v[256];
            if (sscanf(args, "%255s %255s", k, v) == 2) {
                int found = -1;
                for (int i = 0; i < key_count; i++) {
                    if (strcmp(index[i].key, k) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    index = realloc(index, (key_count + 1) * sizeof(KeyList));
                    index[key_count].key = strdup(k);
                    index[key_count].values = NULL;
                    index[key_count].val_count = 0;
                    found = key_count;
                    key_count++;
                }
                index[found].values = realloc(index[found].values, (index[found].val_count + 1) * sizeof(char *));
                index[found].values[index[found].val_count] = strdup(v);
                index[found].val_count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char k[256], v[256];
            if (sscanf(args, "%255s %255s", k, v) == 2) {
                for (int i = 0; i < key_count; i++) {
                    if (strcmp(index[i].key, k) == 0) {
                        for (int j = 0; j < index[i].val_count; j++) {
                            if (strcmp(index[i].values[j], v) == 0) {
                                free(index[i].values[j]);
                                for (int m = j; m < index[i].val_count - 1; m++) {
                                    index[i].values[m] = index[i].values[m + 1];
                                }
                                index[i].val_count--;
                                index[i].values = realloc(index[i].values, index[i].val_count * sizeof(char *));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char k[256];
            if (sscanf(args, "%255s", k) == 1) {
                for (int i = 0; i < key_count; i++) {
                    if (strcmp(index[i].key, k) == 0) {
                        free(index[i].key);
                        for (int j = 0; j < index[i].val_count; j++) {
                            free(index[i].values[j]);
                        }
                        free(index[i].values);
                        for (int j = i; j < key_count - 1; j++) {
                            index[j] = index[j + 1];
                        }
                        key_count--;
                        index = realloc(index, key_count * sizeof(KeyList));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char k[256];
            if (sscanf(args, "%255s", k) == 1) {
                for (int i = 0; i < key_count; i++) {
                    if (strcmp(index[i].key, k) == 0) {
                        for (int j = 0; j < index[i].val_count; j++) {
                            printf("%s%s", index[i].values[j], (j == index[i].val_count - 1) ? "" : " ");
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < key_count; i++) {
                printf("%s\n", index[i].key);
            }
        }
    }
    for (int i = 0; i < key_count; i++) {
        free(index[i].key);
        for (int j = 0; j < index[i].val_count; j++) {
            free(index[i].values[j]);
        }
        free(index[i].values);
    }
    free(index);
    return 0;
}
