// F017.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Config;

int main(int argc, char **argv) {
    Config *list = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while ((read = getline(&line, &len, f)) != -1) {
                if (read > 0 && line[read - 1] == '\n') {
                    line[read - 1] = '\0';
                }
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = line;
                    char *value = eq + 1;
                    Config *tmp = realloc(list, sizeof(Config) * (count + 1));
                    if (tmp) {
                        list = tmp;
                        list[count].key = strdup(key);
                        list[count].value = strdup(value);
                        if (list[count].key && list[count].value) {
                            count++;
                        }
                    }
                }
            }
            fclose(f);
        }
    }

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *key = p;
                char *value = space + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    char *nv = strdup(value);
                    if (nv) {
                        free(list[found].value);
                        list[found].value = nv;
                    }
                } else {
                    Config *tmp = realloc(list, sizeof(Config) * (count + 1));
                    if (tmp) {
                        list = tmp;
                        list[count].key = strdup(key);
                        list[count].value = strdup(value);
                        if (list[count].key && list[count].value) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].key);
                free(list[found].value);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    printf("%s\n", list[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", list[i].key, list[i].value);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    free(line);
    return 0;
}
