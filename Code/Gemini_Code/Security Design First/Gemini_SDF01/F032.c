// F032.c
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
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                free(committed[found].value);
                committed[found].value = strdup(value);
                if (!committed[found].value) exit(1);
            } else {
                if (c_count >= c_capacity) {
                    c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                    Setting *new_c = realloc(committed, c_capacity * sizeof(Setting));
                    if (!new_c) exit(1);
                    committed = new_c;
                }
                committed[c_count].key = strdup(key);
                committed[c_count].value = strdup(value);
                if (!committed[c_count].key || !committed[c_count].value) exit(1);
                c_count++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
            }
            in_tx = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_tx) {
                char *key = p;
                while (*p && *p != ' ' && *p != '	') p++;
                if (*p) {
                    *p = '\0';
                    p++;
                }
                while (*p == ' ' || *p == '	') p++;
                char *value = p;

                int found = -1;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        found = (int)i;
                        break;
                    }
                }

                if (found != -1) {
                    free(staged[found].value);
                    staged[found].value = strdup(value);
                    if (!staged[found].value) exit(1);
                } else {
                    if (s_count >= s_capacity) {
                        s_capacity = s_capacity == 0 ? 4 : s_capacity * 2;
                        Setting *new_s = realloc(staged, s_capacity * sizeof(Setting));
                        if (!new_s) exit(1);
                        staged = new_s;
                    }
                    staged[s_count].key = strdup(key);
                    staged[s_count].value = strdup(value);
                    if (!staged[s_count].key || !staged[s_count].value) exit(1);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    int found = -1;
                    for (size_t j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            found = (int)j;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(committed[found].value);
                        committed[found].value = staged[i].value;
                        free(staged[i].key);
                    } else {
                        if (c_count >= c_capacity) {
                            c_capacity = c_capacity == 0 ? 4 : c_capacity * 2;
                            Setting *new_c = realloc(committed, c_capacity * sizeof(Setting));
                            if (!new_c) exit(1);
                            committed = new_c;
                        }
                        committed[c_count] = staged[i];
                        c_count++;
                    }
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
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

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
            } else {
                printf("NOT_FOUND\n");
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
