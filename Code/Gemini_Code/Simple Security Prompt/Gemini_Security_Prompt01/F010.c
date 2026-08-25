// F010.c
#define _GNU_SOURCE
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
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *key = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *val = space2 + 1;
                while (*val == ' ') val++;

                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(index[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }

                if (found == -1) {
                    index = realloc(index, (count + 1) * sizeof(KeyList));
                    if (!index) return 1;
                    index[count].key = strdup(key);
                    index[count].values = malloc(sizeof(char *));
                    if (!index[count].values) return 1;
                    index[count].values[0] = strdup(val);
                    index[count].val_count = 1;
                    count++;
                } else {
                    int vc = index[found].val_count;
                    index[found].values = realloc(index[found].values, (vc + 1) * sizeof(char *));
                    if (!index[found].values) return 1;
                    index[found].values[vc] = strdup(val);
                    index[found].val_count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && args) {
            char *key = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *val = space2 + 1;
                while (*val == ' ') val++;

                for (int i = 0; i < count; i++) {
                    if (strcmp(index[i].key, key) == 0) {
                        for (int j = 0; j < index[i].val_count; j++) {
                            if (strcmp(index[i].values[j], val) == 0) {
                                free(index[i].values[j]);
                                for (int k = j; k < index[i].val_count - 1; k++) {
                                    index[i].values[k] = index[i].values[k + 1];
                                }
                                index[i].val_count--;
                                index[i].values = realloc(index[i].values, index[i].val_count * sizeof(char *));
                                if (index[i].val_count > 0 && !index[i].values) return 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, args) == 0) {
                    free(index[i].key);
                    for (int j = 0; j < index[i].val_count; j++) {
                        free(index[i].values[j]);
                    }
                    free(index[i].values);
                    for (int j = i; j < count - 1; j++) {
                        index[j] = index[j + 1];
                    }
                    count--;
                    index = realloc(index, count * sizeof(KeyList));
                    if (count > 0 && !index) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, args) == 0) {
                    for (int j = 0; j < index[i].val_count; j++) {
                        printf("%s%s", index[i].values[j], (j == index[i].val_count - 1) ? "" : " ");
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", index[i].key);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(index[i].key);
        for (int j = 0; j < index[i].val_count; j++) {
            free(index[i].values[j]);
        }
        free(index[i].values);
    }
    free(index);
    return 0;
}
