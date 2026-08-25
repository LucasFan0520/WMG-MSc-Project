/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_entry(Entry *e) {
    free(e->key);
    free(e->value);
    free(e);
}

void set_entry(const char *key, const char *value) {
    Entry *e = find_entry(key);
    if (e) {
        free(e->value);
        e->value = strdup(value);
        return;
    }
    e = malloc(sizeof(Entry));
    e->key = strdup(key);
    e->value = strdup(value);
    e->next = entries;
    entries = e;
}

void unset_entry(const char *key) {
    Entry *prev = NULL, *curr = entries;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else entries = curr->next;
            free_entry(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void get_entry(const char *key) {
    Entry *e = find_entry(key);
    if (!e) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", e->value);
}

void write_entries(void) {
    Entry *e = entries;
    while (e) {
        printf("%s=%s\n", e->key, e->value);
        e = e->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        set_entry(key, value);
    }
    fclose(f);
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);

    char line[4096];
    char cmd[32];
    char key[256];
    char value[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) != 1) continue;
            p += strlen(key);
            while (*p == ' ') p++;
            set_entry(key, p);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) == 1) unset_entry(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) == 1) get_entry(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_entries();
        }
    }

    while (entries) {
        Entry *tmp = entries;
        entries = entries->next;
        free_entry(tmp);
    }
    return 0;
}