// F010.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} KeyValue;

int main(void) {
    KeyValue *pairs = NULL;
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
                    KeyValue *next = realloc(pairs, capacity * sizeof(KeyValue));
                    if (!next) break;
                    pairs = next;
                }
                pairs[count].key = strdup(key);
                pairs[count].value = strdup(value);
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *key = args;
                char *value = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(pairs[i].key, key) == 0 && strcmp(pairs[i].value, value) == 0) {
                        free(pairs[i].key);
                        free(pairs[i].value);
                        for (size_t j = i; j < count - 1; j++) {
                            pairs[j] = pairs[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key = line + 6;
            size_t i = 0;
            while (i < count) {
                if (strcmp(pairs[i].key, key) == 0) {
                    free(pairs[i].key);
                    free(pairs[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        pairs[j] = pairs[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(pairs[i].key, key) == 0) {
                    printf("%s\n", pairs[i].value);
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            char **seen = NULL;
            size_t seen_count = 0;
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < seen_count; j++) {
                    if (strcmp(seen[j], pairs[i].key) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", pairs[i].key);
                    seen = realloc(seen, (seen_count + 1) * sizeof(char *));
                    seen[seen_count++] = pairs[i].key;
                }
            }
            free(seen);
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(pairs[i].key);
        free(pairs[i].value);
    }
    free(pairs);
    return 0;
}
