/* F032.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Setting {
    char *key;
    char *value;
} Setting;

static Setting *committed = NULL;
static size_t ccount = 0, ccap = 0;
static Setting *staged = NULL;
static size_t scount = 0, scap = 0;
static int in_transaction = 0;

static Setting *find_setting(Setting *arr, size_t n, const char *key) {
    for (size_t i = 0; i < n; i++) {
        if (strcmp(arr[i].key, key) == 0)
            return &arr[i];
    }
    return NULL;
}

static void add_setting(Setting **arr, size_t *n, size_t *cap, const char *key, const char *value) {
    Setting *existing = find_setting(*arr, *n, key);
    if (existing) {
        free(existing->value);
        existing->value = strdup(value);
        return;
    }
    if (*n == *cap) {
        *cap = *cap ? *cap * 2 : 4;
        *arr = realloc(*arr, *cap * sizeof(Setting));
        if (!*arr) { perror("realloc"); exit(1); }
    }
    (*arr)[*n].key = strdup(key);
    (*arr)[*n].value = strdup(value);
    (*n)++;
}

static void remove_setting(Setting **arr, size_t *n, const char *key) {
    for (size_t i = 0; i < *n; i++) {
        if (strcmp((*arr)[i].key, key) == 0) {
            free((*arr)[i].key);
            free((*arr)[i].value);
            for (size_t j = i; j < *n - 1; j++)
                (*arr)[j] = (*arr)[j + 1];
            (*n)--;
            return;
        }
    }
}

static void begin_transaction(void) {
    if (in_transaction) return;
    // clear staged
    for (size_t i = 0; i < scount; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    scount = 0;
    in_transaction = 1;
}

static void tset(const char *key, const char *value) {
    if (!in_transaction) return;
    add_setting(&staged, &scount, &scap, key, value);
}

static void commit_transaction(void) {
    if (!in_transaction) return;
    for (size_t i = 0; i < scount; i++) {
        add_setting(&committed, &ccount, &ccap, staged[i].key, staged[i].value);
    }
    // clear staged
    for (size_t i = 0; i < scount; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    scount = 0;
    in_transaction = 0;
}

static void abort_transaction(void) {
    if (!in_transaction) return;
    for (size_t i = 0; i < scount; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    scount = 0;
    in_transaction = 0;
}

static void get_setting(const char *key) {
    Setting *s = find_setting(committed, ccount, key);
    if (s) printf("%s\n", s->value);
}

static void dump_settings(void) {
    for (size_t i = 0; i < ccount; i++) {
        printf("%s %s\n", committed[i].key, committed[i].value);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            add_setting(&committed, &ccount, &ccap, key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            tset(key, val);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    for (size_t i = 0; i < ccount; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (size_t i = 0; i < scount; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    return 0;
}