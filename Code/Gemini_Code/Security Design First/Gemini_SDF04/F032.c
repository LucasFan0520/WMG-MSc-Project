// F032.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

Setting *committed = NULL;
int com_count = 0;
int com_cap = 0;

Setting *staged = NULL;
int stg_count = 0;
int stg_cap = 0;

int tx_active = 0;

int find_setting(Setting *arr, int cnt, const char *key) {
    for (int i = 0; i < cnt; i++) {
        if (strcmp(arr[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void set_value(Setting **arr, int *cnt, int *cap, const char *key, const char *val) {
    int idx = find_setting(*arr, *cnt, key);
    if (idx != -1) {
        char *nv = strdup(val);
        if (nv) {
            free((*arr)[idx].value);
            (*arr)[idx].value = nv;
        }
    } else {
        if (*cnt >= *cap) {
            *cap = *cap == 0 ? 4 : *cap * 2;
            Setting *tmp = realloc(*arr, *cap * sizeof(Setting));
            if (!tmp) return;
            *arr = tmp;
        }
        (*arr)[*cnt].key = strdup(key);
        (*arr)[*cnt].value = strdup(val);
        if ((*arr)[*cnt].key && (*arr)[*cnt].value) {
            (*cnt)++;
        }
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "SET") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    set_value(&committed, &com_count, &com_cap, arg1, space2 + 1);
                }
            } else if (strcmp(cmd, "TSET") == 0) {
                if (tx_active) {
                    char *space2 = strchr(arg1, ' ');
                    if (space2) {
                        *space2 = '\0';
                        set_value(&staged, &stg_count, &stg_cap, arg1, space2 + 1);
                    }
                }
            } else if (strcmp(cmd, "GET") == 0) {
                int idx = -1;
                if (tx_active) {
                    idx = find_setting(staged, stg_count, arg1);
                    if (idx != -1) {
                        printf("%s\n", staged[idx].value);
                    }
                }
                if (idx == -1) {
                    idx = find_setting(committed, com_count, arg1);
                    if (idx != -1) {
                        printf("%s\n", committed[idx].value);
                    }
                }
            }
        } else {
            if (strcmp(cmd, "BEGIN") == 0) {
                tx_active = 1;
            } else if (strcmp(cmd, "COMMIT") == 0) {
                if (tx_active) {
                    for (int i = 0; i < stg_count; i++) {
                        set_value(&committed, &com_count, &com_cap, staged[i].key, staged[i].value);
                        free(staged[i].key);
                        free(staged[i].value);
                    }
                    stg_count = 0;
                    tx_active = 0;
                }
            } else if (strcmp(cmd, "ABORT") == 0) {
                if (tx_active) {
                    for (int i = 0; i < stg_count; i++) {
                        free(staged[i].key);
                        free(staged[i].value);
                    }
                    stg_count = 0;
                    tx_active = 0;
                }
            } else if (strcmp(cmd, "DUMP") == 0) {
                for (int i = 0; i < com_count; i++) {
                    printf("%s=%s\n", committed[i].key, committed[i].value);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < com_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (int i = 0; i < stg_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    return 0;
}