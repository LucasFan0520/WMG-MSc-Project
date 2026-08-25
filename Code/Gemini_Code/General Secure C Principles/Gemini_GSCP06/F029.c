// F029.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

int main(void) {
    Record *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *key = args;
                char *value = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Record *next = realloc(list, capacity * sizeof(Record));
                    if (!next) break;
                    list = next;
                }
                list[count].key = strdup(key);
                list[count].value = strdup(value);
                count++;
            }
        } else if (strcmp(line, "DEDUP") == 0) {
            size_t i = 0;
            while (i < count) {
                int is_dup = 0;
                for (size_t k = 0; k < i; k++) {
                    if (strcmp(list[k].key, list[i].key) == 0) {
                        is_dup = 1;
                        break;
                    }
                }
                if (is_dup) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *key = line + 7;
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *key = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    printf("%s\n", list[i].value);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].key, list[i].value);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    free(line);
    return 0;
}
