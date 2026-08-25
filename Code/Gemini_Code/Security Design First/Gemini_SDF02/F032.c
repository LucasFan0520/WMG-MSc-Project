/* F032.c */
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

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *value = p;

            int found = 0;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    char *temp = strdup(value);
                    if (temp) {
                        free(committed[i].value);
                        committed[i].value = temp;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (c_count >= c_capacity) {
                    size_t new_cap = c_capacity == 0 ? 4 : c_capacity * 2;
                    Setting *temp = realloc(committed, new_cap * sizeof(Setting));
                    if (!temp) break;
                    committed = temp;
                    c_capacity = new_cap;
                }
                committed[c_count].key = strdup(key);
                committed[c_count].value = strdup(value);
                if (committed[c_count].key && committed[c_count].value) c_count++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                char *key = p;
                while (*p && *p != ' ') p++;
                if (*p) { *p = '\0'; p++; }
                while (*p == ' ') p++;
                char *value = p;

                int found = 0;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        char *temp = strdup(value);
                        if (temp) {
                            free(staged[i].value);
                            staged[i].value = temp;
                        }
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (s_count >= s_capacity) {
                        size_t new_cap = s_capacity == 0 ? 4 : s_capacity * 2;
                        Setting *temp = realloc(staged, new_cap * sizeof(Setting));
                        if (!temp) break;
                        staged = temp;
                        s_capacity = new_cap;
                    }
                    staged[s_count].key = strdup(key);
                    staged[s_count].value = strdup(value);
                    if (staged[s_count].key && staged[s_count].value) s_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = 0;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            char *temp = strdup(staged[i].value);
                            if (temp) {
                                free(committed[j].value);
                                committed[j].value = temp;
                            }
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        if (c_count >= c_capacity) {
                            size_t new_cap = c_capacity == 0 ? 4 : c_capacity * 2;
                            Setting *temp = realloc(committed, new_cap * sizeof(Setting));
                            if (!temp) break;
                            committed = temp;
                            c_capacity = new_cap;
                        }
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        if (committed[c_count].key && committed[c_count].value) c_count++;
                    }
                }
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = p;
            int found = 0;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        printf("%s\n", staged[i].value);
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        printf("%s\n", committed[i].value);
                        found = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s %s\n", committed[i].key, committed[i].value);
            }
        }
    }
    free(line);
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
    return 0;
}
