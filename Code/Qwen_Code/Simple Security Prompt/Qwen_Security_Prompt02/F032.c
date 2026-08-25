// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

Setting *committed = NULL;
int c_count = 0;

Setting *staged = NULL;
int s_count = 0;

int in_transaction = 0;

int find_committed(const char *key) {
    for (int i = 0; i < c_count; i++) {
        if (strcmp(committed[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int find_staged(const char *key) {
    for (int i = 0; i < s_count; i++) {
        if (strcmp(staged[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char key[1024];
    char value[10240];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key);
            fgets(value, sizeof(value), stdin);
            if (value[0] == ' ') {
                memmove(value, value + 1, strlen(value));
            }
            value[strcspn(value, "\n")] = '\0';
            int idx = find_committed(key);
            if (idx != -1) {
                free(committed[idx].value);
                committed[idx].value = strdup(value);
            } else {
                committed = realloc(committed, sizeof(Setting) * (c_count + 1));
                committed[c_count].key = strdup(key);
                committed[c_count].value = strdup(value);
                c_count++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            scanf("%s", key);
            fgets(value, sizeof(value), stdin);
            if (value[0] == ' ') {
                memmove(value, value + 1, strlen(value));
            }
            value[strcspn(value, "\n")] = '\0';
            if (in_transaction) {
                int idx = find_staged(key);
                if (idx != -1) {
                    free(staged[idx].value);
                    staged[idx].value = strdup(value);
                } else {
                    staged = realloc(staged, sizeof(Setting) * (s_count + 1));
                    staged[s_count].key = strdup(key);
                    staged[s_count].value = strdup(value);
                    s_count++;
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_transaction) {
                for (int i = 0; i < s_count; i++) {
                    int idx = find_committed(staged[i].key);
                    if (idx != -1) {
                        free(committed[idx].value);
                        committed[idx].value = strdup(staged[i].value);
                    } else {
                        committed = realloc(committed, sizeof(Setting) * (c_count + 1));
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
                in_transaction = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_transaction) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                s_count = 0;
                in_transaction = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s", key);
            int idx = find_committed(key);
            if (idx != -1) {
                printf("%s\n", committed[idx].value);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s %s\n", committed[i].key, committed[i].value);
            }
        }
    }

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