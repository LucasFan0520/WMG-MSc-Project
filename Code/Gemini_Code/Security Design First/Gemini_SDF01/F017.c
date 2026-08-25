// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

int main(int argc, char **argv) {
    if (argc < 2) return 1;

    ConfigPair *items = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        long fread_bytes;
        while ((fread_bytes = getline(&fline, &flen, f)) != -1) {
            if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                fline[fread_bytes - 1] = '\0';
                fread_bytes--;
            }
            if (fread_bytes > 0 && fline[fread_bytes - 1] == '\n') {
                fline[fread_bytes - 1] = '\0';
                fread_bytes--;
            }

            char *eq = strchr(fline, '=');
            if (eq) {
                *eq = '\0';
                char *key = fline;
                char *value = eq + 1;

                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    ConfigPair *new_items = realloc(items, capacity * sizeof(ConfigPair));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].key = strdup(key);
                items[count].value = strdup(value);
                if (!items[count].key || !items[count].value) exit(1);
                count++;
            }
        }
        free(fline);
        fclose(f);
    }

    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *value = p;

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                free(items[found].value);
                items[found].value = strdup(value);
                if (!items[found].value) exit(1);
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    ConfigPair *new_items = realloc(items, capacity * sizeof(ConfigPair));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].key = strdup(key);
                items[count].value = strdup(value);
                if (!items[count].key || !items[count].value) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    free(items[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", items[found].value);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", items[i].key, items[i].value);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].key);
        free(items[i].value);
    }
    free(items);
    free(line);
    return 0;
}
