// F010.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **values;
    size_t val_count;
    size_t val_cap;
} KeyValues;

typedef struct {
    char *key;
    KeyValues list;
} KeyEntry;

static KeyEntry *entries = NULL;
static size_t entry_count = 0;
static size_t entry_cap = 0;

static ssize_t find_entry(const char *key) {
    for (size_t i = 0; i < entry_count; i++)
        if (strcmp(entries[i].key, key) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_value(KeyValues *kv, const char *val) {
    if (kv->val_count == kv->val_cap) {
        kv->val_cap = kv->val_cap ? kv->val_cap * 2 : 4;
        char **new = realloc(kv->values, kv->val_cap * sizeof(char *));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        kv->values = new;
    }
    kv->values[kv->val_count] = strdup(val);
    if (!kv->values[kv->val_count]) { fprintf(stderr, "Memory error\n"); exit(1); }
    kv->val_count++;
}

static void init_kv(KeyValues *kv) {
    kv->values = NULL;
    kv->val_count = 0;
    kv->val_cap = 0;
}

static void free_kv(KeyValues *kv) {
    for (size_t i = 0; i < kv->val_count; i++)
        free(kv->values[i]);
    free(kv->values);
}

static void add_entry(const char *key, const char *val) {
    ssize_t idx = find_entry(key);
    if (idx != -1) {
        add_value(&entries[idx].list, val);
        return;
    }
    if (entry_count == entry_cap) {
        entry_cap = entry_cap ? entry_cap * 2 : 4;
        KeyEntry *new = realloc(entries, entry_cap * sizeof(KeyEntry));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        entries = new;
    }
    entries[entry_count].key = strdup(key);
    if (!entries[entry_count].key) { fprintf(stderr, "Memory error\n"); exit(1); }
    init_kv(&entries[entry_count].list);
    add_value(&entries[entry_count].list, val);
    entry_count++;
}

static void remove_value(const char *key, const char *val) {
    ssize_t idx = find_entry(key);
    if (idx == -1) return;
    KeyValues *kv = &entries[idx].list;
    size_t write = 0;
    for (size_t i = 0; i < kv->val_count; i++) {
        if (strcmp(kv->values[i], val) == 0) {
            free(kv->values[i]);
        } else {
            if (write != i) kv->values[write] = kv->values[i];
            write++;
        }
    }
    kv->val_count = write;
}

static void erase_entry(const char *key) {
    ssize_t idx = find_entry(key);
    if (idx == -1) return;
    free(entries[idx].key);
    free_kv(&entries[idx].list);
    for (size_t i = idx; i < entry_count - 1; i++)
        entries[i] = entries[i + 1];
    entry_count--;
}

static void values_print(const char *key) {
    ssize_t idx = find_entry(key);
    if (idx == -1) return;
    for (size_t i = 0; i < entries[idx].list.val_count; i++)
        printf("%s\n", entries[idx].list.values[i]);
}

static void keys_print(void) {
    for (size_t i = 0; i < entry_count; i++)
        printf("%s\n", entries[i].key);
}

static void free_all(void) {
    for (size_t i = 0; i < entry_count; i++) {
        free(entries[i].key);
        free_kv(&entries[i].list);
    }
    free(entries);
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

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) == 2)
                add_entry(key, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) == 2)
                remove_value(key, val);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                erase_entry(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                values_print(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_print();
        }
    }

    free(line);
    free_all();
    return 0;
}