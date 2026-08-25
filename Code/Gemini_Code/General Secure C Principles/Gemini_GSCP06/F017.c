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
    if (argc < 2) return 1;
    Config *list = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread;
        while ((fread = getline(&fline, &flen, f)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            char *p = strchr(fline, '=');
            if (p) {
                *p = '\0';
                char *key = fline;
                char *value = p + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Config *next = realloc(list, capacity * sizeof(Config));
                    if (!next) break;
                    list = next;
                }
                list[count].key = strdup(key);
                list[count].value = strdup(value);
                count++;
            }
        }
        free(fline);
        fclose(f);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *key = args;
                char *value = space + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].value);
                    list[found].value = strdup(value);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Config *next = realloc(list, capacity * sizeof(Config));
                        if (!next) break;
                        list = next;
                    }
                    list[count].key = strdup(key);
                    list[count].value = strdup(value);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    printf("%s\n", list[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", list[i].key, list[i].value);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    return 0;
}
