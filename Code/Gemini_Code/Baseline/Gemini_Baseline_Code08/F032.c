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
int com_capacity = 0;

Setting *staged = NULL;
int stg_count = 0;
int stg_capacity = 0;

int in_transaction = 0;

int find_setting(Setting *arr, int count, const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(arr[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void set_committed(const char *key, const char *value) {
    int idx = find_setting(committed, com_count, key);
    if (idx != -1) {
        free(committed[idx].value);
        committed[idx].value = strdup(value);
        return;
    }
    if (com_count >= com_capacity) {
        com_capacity = com_capacity == 0 ? 4 : com_capacity * 2;
        committed = realloc(committed, com_capacity * sizeof(Setting));
    }
    committed[com_count].key = strdup(key);
    committed[com_count].value = strdup(value);
    com_count++;
}

void set_staged(const char *key, const char *value) {
    int idx = find_setting(staged, stg_count, key);
    if (idx != -1) {
        free(staged[idx].value);
        staged[idx].value = strdup(value);
        return;
    }
    if (stg_count >= stg_capacity) {
        stg_capacity = stg_capacity == 0 ? 4 : stg_capacity * 2;
        staged = realloc(staged, stg_capacity * sizeof(Setting));
    }
    staged[stg_count].key = strdup(key);
    staged[stg_count].value = strdup(value);
    stg_count++;
}

void commit_transaction(void) {
    if (!in_transaction) return;
    for (int i = 0; i < stg_count; i++) {
        set_committed(staged[i].key, staged[i].value);
        free(staged[i].key);
        free(staged[i].value);
    }
    stg_count = 0;
    in_transaction = 0;
}

void abort_transaction(void) {
    if (!in_transaction) return;
    for (int i = 0; i < stg_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    stg_count = 0;
    in_transaction = 0;
}

void get_setting(const char *key) {
    if (in_transaction) {
        int idx = find_setting(staged, stg_count, key);
        if (idx != -1) {
            printf("%s\n", staged[idx].value);
            return;
        }
    }
    int idx = find_setting(committed, com_count, key);
    if (idx != -1) {
        printf("%s\n", committed[idx].value);
    }
}

void dump_settings(void) {
    for (int i = 0; i < com_count; i++) {
        printf("%s=%s\n", committed[i].key, committed[i].value);
    }
}

int main(void) {
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
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");
            if (key && value) set_committed(key, value);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");
            if (in_transaction && key && value) set_staged(key, value);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, "");
            if (key) get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    free(line);
    abort_transaction();
    free(staged);
    for (int i = 0; i < com_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    return 0;
}
