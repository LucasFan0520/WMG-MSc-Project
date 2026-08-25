// F032.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

int main(void) {
    Setting *committed = NULL;
    int c_count = 0;
    int c_capacity = 0;

    Setting *staged = NULL;
    int s_count = 0;
    int s_capacity = 0;
    int in_tx = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            char *val = "";
            if (space) {
                *space = '\0';
                val = space + 1;
                while (*val == ' ') val++;
            }
            int found = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(committed[found].value);
                committed[found].value = strdup(val);
            } else {
                if (c_count >= c_capacity) {
                    int new_cap = c_capacity == 0 ? 4 : c_capacity * 2;
                    Setting *tmp = realloc(committed, new_cap * sizeof(Setting));
                    if (!tmp) return 1;
                    committed = tmp;
                    c_capacity = new_cap;
                }
                committed[c_count].key = strdup(key);
                committed[c_count].value = strdup(val);
                c_count++;
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            in_tx = 1;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (!in_tx) continue;
            char *p = line + 5;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            char *val = "";
            if (space) {
                *space = '\0';
                val = space + 1;
                while (*val == ' ') val++;
            }
            int found = -1;
            for (int i = 0; i < s_count; i++) {
                if (strcmp(staged[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(staged[found].value);
                staged[found].value = strdup(val);
            } else {
                if (s_count >= s_capacity) {
                    int new_cap = s_capacity == 0 ? 4 : s_capacity * 2;
                    Setting *tmp = realloc(staged, new_cap * sizeof(Setting));
                    if (!tmp) return 1;
                    staged = tmp;
                    s_capacity = new_cap;
                }
                staged[s_count].key = strdup(key);
                staged[s_count].value = strdup(val);
                s_count++;
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (!in_tx) continue;
            for (int i = 0; i < s_count; i++) {
                int found = -1;
                for (int j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, staged[i].key) == 0) {
                        found = j;
                        break;
                    }
                }
                if (found != -1) {
                    free(committed[found].value);
                    committed[found].value = strdup(staged[i].value);
                } else {
                    if (c_count >= c_capacity) {
                        int new_cap = c_capacity == 0 ? 4 : c_capacity * 2;
                        Setting *tmp = realloc(committed, new_cap * sizeof(Setting));
                        if (!tmp) return 1;
                        committed = tmp;
                        c_capacity = new_cap;
                    }
                    committed[c_count].key = strdup(staged[i].key);
                    committed[c_count].value = strdup(staged[i].value);
                    c_count++;
                }
            }
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_tx = 0;
        } else if (strcmp(line, "ABORT") == 0) {
            if (!in_tx) continue;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_tx = 0;
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            int found = -1;
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", staged[found].value);
                    continue;
                }
            }
            found = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%s\n", committed[found].value);
            else printf("NOT_FOUND\n");
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
    }
    free(line);
    for (int i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (int i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    return 0;
}
