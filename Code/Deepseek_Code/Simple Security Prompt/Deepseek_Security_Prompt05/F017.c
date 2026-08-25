/* F017.c */
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
        return;
    }
    e = malloc(sizeof(Entry));
    if (!e) return;
    e->key = strdup(key);
    e->value = strdup(value);
    e->next = entries;
    entries = e;
}

void unset_entry(const char *key) {
    Entry *e = entries, *prev = NULL;
    while (e) {
        if (strcmp(e->key, key) == 0) {
            if (prev) prev->next = e->next;
            else entries = e->next;
            free(e->key);
            free(e->value);
            free(e);
            return;
        }
        prev = e;
        e = e->next;
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

void write_all(void) {
    Entry *e = entries;
    while (e) {
        printf("%s=%s\n", e->key, e->value);
        e = e->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char *eq = strchr(line, '=');
                if (!eq) continue;
                *eq = '\0';
                set_entry(line, eq + 1);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *val = line + strlen(cmd) + strlen(arg1) + 2;
            while (*val == ' ') val++;
            set_entry(arg1, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_entry(arg1);
        } else if (strcmp(cmd, "GET") == 0) {
            get_entry(arg1);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_all();
        }
    }
    while (entries) {
        Entry *tmp = entries;
        entries = entries->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    return 0;
}