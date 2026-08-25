// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    char *key;
    char *value;
    struct Entry *next;
} Entry;

Entry *entries = NULL;

Entry *find_entry(const char *key) {
    Entry *e = entries;
    while (e) {
        if (strcmp(e->key, key) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void set_entry(const char *key, const char *value) {
    Entry *e = find_entry(key);
    if (e) {
        free(e->value);
        e->value = strdup(value);
    } else {
        Entry *new = malloc(sizeof(Entry));
        if (!new) return;
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = entries;
        entries = new;
    }
}

void unset_entry(const char *key) {
    Entry *prev = NULL, *cur = entries;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else entries = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void get_entry(const char *key) {
    Entry *e = find_entry(key);
    if (e) printf("%s\n", e->value);
}

void write_entries(void) {
    Entry *e = entries;
    while (e) {
        printf("%s=%s\n", e->key, e->value);
        e = e->next;
    }
}

void free_all(void) {
    Entry *e = entries;
    while (e) {
        Entry *next = e->next;
        free(e->key);
        free(e->value);
        free(e);
        e = next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing file argument\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        set_entry(key, val);
    }
    fclose(f);

    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            set_entry(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (space) *space = '\0';
            unset_entry(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (space) *space = '\0';
            get_entry(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_entries();
        }
    }
    free_all();
    return 0;
}