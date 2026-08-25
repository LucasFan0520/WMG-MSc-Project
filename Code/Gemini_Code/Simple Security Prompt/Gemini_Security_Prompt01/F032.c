// F032.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

int main() {
    Setting *committed = NULL;
    int c_count = 0;
    Setting *staged = NULL;
    int s_count = 0;
    int in_tx = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "SET") == 0 && args) {
            char *key = args;
            char *space2 = strchr(args, ' ');
            char *value = "";
            if (space2) {
                *space2 = '\0';
                value = space2 + 1;
                while (*value == ' ') value++;
            }
            int found = 0;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    free(committed[i].value);
                    committed[i].value = strdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                if (!committed) return 1;
                committed[c_count].key = strdup(key);
                committed[c_count].value = strdup(value);
                c_count++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            free(staged);
            staged = NULL;
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0 && args) {
            if (in_tx) {
                char *key = args;
                char *space2 = strchr(args, ' ');
                char *value = "";
                if (space2) {
                    *space2 = '\0';
                    value = space2 + 1;
                    while (*value == ' ') value++;
                }
                int found = 0;
                for (int i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        free(staged[i].value);
                        staged[i].value = strdup(value);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    staged = realloc(staged, (s_count + 1) * sizeof(Setting));
                    if (!staged) return 1;
                    staged[s_count].key = strdup(key);
                    staged[s_count].value = strdup(value);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    int found = 0;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            free(committed[j].value);
                            committed[j].value = strdup(staged[i].value);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        committed = realloc(committed, (c_count + 1) * sizeof(Setting));
                        if (!committed) return 1;
                        committed[c_count].key = strdup(staged[i].key);
                        committed[c_count].value = strdup(staged[i].value);
                        c_count++;
                    }
                }
                in_tx = 0;
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_tx) {
                in_tx = 0;
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
            }
        } else if (strcmp(cmd, "GET") == 0 && args) {
            char *val = NULL;
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, args) == 0) {
                        val = staged[i].value;
                        break;
                    }
                }
            }
            if (!val) {
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, args) == 0) {
                        val = committed[i].value;
                        break;
                    }
                }
            }
            if (val) {
                printf("%s\n", val);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s %s\n", committed[i].key, committed[i].value);
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
