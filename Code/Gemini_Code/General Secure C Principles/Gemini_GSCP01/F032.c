// F032.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} SettingPair;

int main(void) {
    SettingPair *committed = NULL;
    size_t c_count = 0;
    SettingPair *staged = NULL;
    size_t s_count = 0;
    int in_tx = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *val = p;
            int found = 0;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    free(committed[i].value);
                    committed[i].value = strdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                SettingPair *temp = realloc(committed, (c_count + 1) * sizeof(SettingPair));
                if (temp) {
                    committed = temp;
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(val);
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            free(staged);
            staged = NULL;
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                while (*p == ' ' || *p == '\t') {
                    p++;
                }
                char *key = p;
                while (*p && *p != ' ' && *p != '\t') {
                    p++;
                }
                if (*p) {
                    *p = '\0';
                    p++;
                }
                while (*p == ' ' || *p == '\t') {
                    p++;
                }
                char *val = p;
                int found = 0;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        free(staged[i].value);
                        staged[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    SettingPair *temp = realloc(staged, (s_count + 1) * sizeof(SettingPair));
                    if (temp) {
                        staged = temp;
                        staged[s_count].key = strdup(key);
                        staged[s_count].value = strdup(val);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = 0;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = strdup(staged[i].value);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        SettingPair *temp = realloc(committed, (c_count + 1) * sizeof(SettingPair));
                        if (temp) {
                            committed = temp;
                            committed[c_count].key = strdup(staged[i].key);
                            committed[c_count].value = strdup(staged[i].value);
                            c_count++;
                        }
                    }
                }
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int printed = 0;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        printf("%s\n", staged[i].value);
                        printed = 1;
                        break;
                    }
                }
            }
            if (!printed) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        printf("%s\n", committed[i].value);
                        printed = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
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
