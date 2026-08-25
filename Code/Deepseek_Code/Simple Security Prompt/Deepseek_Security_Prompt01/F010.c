// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *value; } Val;
typedef struct { char *key; Val *vals; int v_count, v_cap; } Entry;
Entry *entries = NULL;
int entry_count = 0, entry_cap = 0;

int find_entry(const char *key) {
    for (int i = 0; i < entry_count; i++)
        if (strcmp(entries[i].key, key) == 0) return i;
    return -1;
}

void add_value(const char *key, const char *value) {
    int idx = find_entry(key);
    if (idx < 0) {
        if (entry_count == entry_cap) {
            entry_cap = entry_cap ? entry_cap * 2 : 4;
            entries = realloc(entries, sizeof(Entry) * entry_cap);
        }
        entries[entry_count].key = strdup(key);
        entries[entry_count].vals = NULL;
        entries[entry_count].v_count = 0;
        entries[entry_count].v_cap = 0;
        idx = entry_count++;
    }
    Entry *e = &entries[idx];
    if (e->v_count == e->v_cap) {
        e->v_cap = e->v_cap ? e->v_cap * 2 : 4;
        e->vals = realloc(e->vals, sizeof(Val) * e->v_cap);
    }
    e->vals[e->v_count].value = strdup(value);
    e->v_count++;
}

void remove_value(const char *key, const char *value) {
    int idx = find_entry(key);
    if (idx < 0) return;
    Entry *e = &entries[idx];
    for (int i = 0; i < e->v_count; i++) {
        if (strcmp(e->vals[i].value, value) == 0) {
            free(e->vals[i].value);
            for (int j = i; j < e->v_count - 1; j++) e->vals[j] = e->vals[j + 1];
            e->v_count--;
            return;
        }
    }
}

void erase_key(const char *key) {
    int idx = find_entry(key);
    if (idx < 0) return;
    for (int i = 0; i < entries[idx].v_count; i++) free(entries[idx].vals[i].value);
    free(entries[idx].vals);
    free(entries[idx].key);
    for (int i = idx; i < entry_count - 1; i++) entries[i] = entries[i + 1];
    entry_count--;
}

void values_key(const char *key) {
    int idx = find_entry(key);
    if (idx < 0) return;
    for (int i = 0; i < entries[idx].v_count; i++)
        printf("%s\n", entries[idx].vals[i].value);
}

void keys_list(void) {
    for (int i = 0; i < entry_count; i++) printf("%s\n", entries[i].key);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                add_value(arg1, arg2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                remove_value(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) values_key(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) keys_list();
    }
    for (int i = 0; i < entry_count; i++) {
        for (int j = 0; j < entries[i].v_count; j++) free(entries[i].vals[j].value);
        free(entries[i].vals);
        free(entries[i].key);
    }
    free(entries);
    return 0;
}