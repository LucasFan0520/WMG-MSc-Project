// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry { char *key; char *val; struct Entry *next; } Entry;
Entry *entries = NULL;

Entry *find_entry(const char *key) {
    for (Entry *e = entries; e; e = e->next)
        if (strcmp(e->key, key) == 0) return e;
    return NULL;
}

void set_entry(const char *key, const char *val) {
    Entry *e = find_entry(key);
    if (e) { free(e->val); e->val = strdup(val); return; }
    e = malloc(sizeof(Entry));
    if (!e) return;
    e->key = strdup(key); e->val = strdup(val); e->next = entries; entries = e;
}

void unset_entry(const char *key) {
    Entry *prev = NULL;
    for (Entry *e = entries; e; prev = e, e = e->next) {
        if (strcmp(e->key, key) == 0) {
            if (prev) prev->next = e->next; else entries = e->next;
            free(e->key); free(e->val); free(e);
            return;
        }
    }
}

void get_entry(const char *key) {
    Entry *e = find_entry(key);
    if (e) printf("%s\n", e->val);
}

void write_entries(void) {
    for (Entry *e = entries; e; e = e->next) printf("%s=%s\n", e->key, e->val);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *eq = strchr(line, '=');
                if (!eq) continue;
                *eq = 0;
                char *key = line;
                char *val = eq + 1;
                set_entry(key, val);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s", key) == 1) {
                char *v = rest + strlen(key);
                while (*v == ' ') v++;
                strcpy(val, v);
                set_entry(key, val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) unset_entry(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) get_entry(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_entries();
        }
    }
    while (entries) { Entry *e = entries; entries = e->next; free(e->key); free(e->val); free(e); }
    return 0;
}