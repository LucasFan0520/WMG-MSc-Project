// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    char *key;
    char *value;
    struct Entry *next;
} Entry;

Entry *head = NULL;

Entry* find_entry(const char *key) {
    Entry *e = head;
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

void set_value(const char *key, const char *value) {
    Entry *e = find_entry(key);
    if (e) {
        free(e->value);
        e->value = strdup(value);
    } else {
        Entry *new = malloc(sizeof(Entry));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = NULL;
        if (!head) head = new;
        else {
            Entry *last = head;
            while (last->next) last = last->next;
            last->next = new;
        }
    }
}

void unset_key(const char *key) {
    Entry *e = head, *prev = NULL;
    while (e) {
        if (strcmp(e->key, key) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            free_entry(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void get_key(const char *key) {
    Entry *e = find_entry(key);
    if (e) printf("%s\n", e->value);
}

void write_all() {
    Entry *e = head;
    while (e) {
        printf("%s=%s\n", e->key, e->value);
        e = e->next;
    }
}

int main(int argc, char **argv) {
    if (argc == 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = line;
                    char *value = eq + 1;
                    set_value(key, value);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (key) {
                if (!val) val = "";
                set_value(key, val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_key(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) get_key(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_all();
        }
    }
    while (head) {
        Entry *tmp = head;
        head = head->next;
        free_entry(tmp);
    }
    return 0;
}