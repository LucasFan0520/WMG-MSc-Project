// F032.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

void set_setting(Setting **list, size_t *count, size_t *capacity, const char *key, const char *value) {
    for (size_t i = 0; i < *count; i++) {
        if (strcmp((*list)[i].key, key) == 0) {
            free((*list)[i].value);
            (*list)[i].value = strdup(value);
            return;
        }
    }
    if (*count >= *capacity) {
        *capacity = *capacity == 0 ? 4 : *capacity * 2;
        Setting *next = realloc(*list, *capacity * sizeof(Setting));
        if (!next) return;
        *list = next;
    }
    (*list)[*count].key = strdup(key);
    (*list)[*count].value = strdup(value);
    (*count)++;
}

int main(void) {
    Setting *committed = NULL;
    size_t c_count = 0;
    size_t c_capacity = 0;
    Setting *staged = NULL;
    size_t s_count = 0;
    size_t s_capacity = 0;
    int in_tx = 0;

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
                set_setting(&committed, &c_count, &c_capacity, key, value);
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            in_tx = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_tx) {
                char *args = line + 5;
                char *space = strchr(args, ' ');
                if (space) {
                    *space = '\0';
                    char *key = args;
                    char *value = space + 1;
                    set_setting(&staged, &s_count, &s_capacity, key, value);
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    set_setting(&committed, &c_count, &c_capacity, staged[i].key, staged[i].value);
                }
                in_tx = 0;
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_tx) {
                in_tx = 0;
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            char *val = NULL;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        val = staged[i].value;
                        break;
                    }
                }
            }
            if (!val) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        val = committed[i].value;
                        break;
                    }
                }
            }
            if (val) {
                printf("%s\n", val);
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s %s\n", committed[i].key, committed[i].value);
            }
        }
    }
    for (size_t i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (size_t i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    free(line);
    return 0;
}
