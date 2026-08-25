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
            while (*p && *p != ' ') p++;
            char *val = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                val = p;
            }
            int found = 0;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    free(committed[i].value);
                    committed[i].value = strdup(val);
                    if (!committed[i].value) exit(1);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (c_count >= c_capacity) {
                    c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                    Setting *tmp = realloc(committed, c_capacity * sizeof(Setting));
                    if (!tmp) exit(1);
                    committed = tmp;
                }
                committed[c_count].key = strdup(key);
                committed[c_count].value = strdup(val);
                if (!committed[c_count].key || !committed[c_count].value) exit(1);
                c_count++;
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
                char *p = line + 5;
                while (*p == ' ') p++;
                char *key = p;
                while (*p && *p != ' ') p++;
                char *val = "";
                if (*p) {
                    *p = '\0';
                    p++;
                    while (*p == ' ') p++;
                    val = p;
                }
                int found = 0;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        free(staged[i].value);
                        staged[i].value = strdup(val);
                        if (!staged[i].value) exit(1);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (s_count >= s_capacity) {
                        s_capacity = s_capacity == 0 ? 4 : s_capacity * 2;
                        Setting *tmp = realloc(staged, s_capacity * sizeof(Setting));
                        if (!tmp) exit(1);
                        staged = tmp;
                    }
                    staged[s_count].key = strdup(key);
                    staged[s_count].value = strdup(val);
                    if (!staged[s_count].key || !staged[s_count].value) exit(1);
                    s_count++;
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = 0;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = strdup(staged[i].value);
                            if (!committed[j].value) exit(1);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (c_count >= c_capacity) {
                            c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                            Setting *tmp = realloc(committed, c_capacity * sizeof(Setting));
                            if (!tmp) exit(1);
                            committed = tmp;
                        }
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        if (!committed[c_count].key || !committed[c_count].value) exit(1);
                        c_count++;
                    }
                }
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            int s_found = 0;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        printf("%s\n", staged[i].value);
                        s_found = 1;
                        break;
                    }
                }
            }
            if (!s_found) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        printf("%s\n", committed[i].value);
                        s_found = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
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
