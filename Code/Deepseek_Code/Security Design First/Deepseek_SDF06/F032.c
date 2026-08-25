// F032.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Setting {
    char *key;
    char *value;
} Setting;

static Setting *committed = NULL;
static size_t comm_count = 0;
static size_t comm_cap = 0;

static Setting *staged = NULL;
static size_t staged_count = 0;
static size_t staged_cap = 0;

static int in_transaction = 0;

static ssize_t find_setting(Setting *list, size_t n, const char *key) {
    for (size_t i = 0; i < n; i++)
        if (strcmp(list[i].key, key) == 0)
            return (ssize_t)i;
    return -1;
}

static void set_committed(const char *key, const char *value) {
    ssize_t idx = find_setting(committed, comm_count, key);
    if (idx != -1) {
        free(committed[idx].value);
        committed[idx].value = strdup(value);
        if (!committed[idx].value) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    if (comm_count == comm_cap) {
        comm_cap = comm_cap ? comm_cap * 2 : 4;
        Setting *new = realloc(committed, comm_cap * sizeof(Setting));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        committed = new;
    }
    committed[comm_count].key = strdup(key);
    committed[comm_count].value = strdup(value);
    if (!committed[comm_count].key || !committed[comm_count].value)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    comm_count++;
}

static void set_staged(const char *key, const char *value) {
    ssize_t idx = find_setting(staged, staged_count, key);
    if (idx != -1) {
        free(staged[idx].value);
        staged[idx].value = strdup(value);
        if (!staged[idx].value) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    if (staged_count == staged_cap) {
        staged_cap = staged_cap ? staged_cap * 2 : 4;
        Setting *new = realloc(staged, staged_cap * sizeof(Setting));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        staged = new;
    }
    staged[staged_count].key = strdup(key);
    staged[staged_count].value = strdup(value);
    if (!staged[staged_count].key || !staged[staged_count].value)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    staged_count++;
}

static void begin_transaction(void) {
    if (in_transaction) {
        // ignore or discard previous? we'll just ignore
    }
    // Clear any old staged
    for (size_t i = 0; i < staged_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    staged_count = 0;
    in_transaction = 1;
}

static void commit_transaction(void) {
    if (!in_transaction) return;
    // Apply staged to committed
    for (size_t i = 0; i < staged_count; i++) {
        set_committed(staged[i].key, staged[i].value);
    }
    // Clear staged
    for (size_t i = 0; i < staged_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    staged_count = 0;
    in_transaction = 0;
}

static void abort_transaction(void) {
    if (!in_transaction) return;
    for (size_t i = 0; i < staged_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    staged_count = 0;
    in_transaction = 0;
}

static void get_key(const char *key) {
    // Check staged first, then committed
    ssize_t idx = find_setting(staged, staged_count, key);
    if (idx != -1) {
        printf("%s\n", staged[idx].value);
        return;
    }
    idx = find_setting(committed, comm_count, key);
    if (idx != -1) {
        printf("%s\n", committed[idx].value);
    }
}

static void dump_committed(void) {
    for (size_t i = 0; i < comm_count; i++)
        printf("%s %s\n", committed[i].key, committed[i].value);
}

static void free_all(void) {
    for (size_t i = 0; i < comm_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (size_t i = 0; i < staged_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], key[256], val[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) == 2) {
                if (in_transaction) {
                    // staged? SET immediate? Spec says SET changes immediately, even in transaction
                    set_committed(key, val);
                } else {
                    set_committed(key, val);
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_transaction) {
                if (sscanf(line, "%*s %255s %4095[^\n]", key, val) == 2)
                    set_staged(key, val);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                get_key(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_committed();
        }
    }

    free(line);
    free_all();
    return 0;
}