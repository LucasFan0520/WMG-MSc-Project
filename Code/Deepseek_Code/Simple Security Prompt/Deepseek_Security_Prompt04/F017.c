/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
    char *key;
    char *value;
    struct Entry *next;
} Entry;

Entry *head = NULL;

Entry *find_entry(char *key) {
    for (Entry *e = head; e; e = e->next)
        if (strcmp(e->key, key) == 0) return e;
    return NULL;
}

void set_entry(char *key, char *value) {
    Entry *e = find_entry(key);
    if (e) {
        free(e->value);
        e->value = strdup(value);
    } else {
        Entry *new = malloc(sizeof(Entry));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = head;
        head = new;
    }
}

void unset_entry(char *key) {
    Entry *prev = NULL;
    for (Entry *e = head; e; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            free(e->key);
            free(e->value);
            free(e);
            return;
        }
        prev = e;
    }
}

void get_entry(char *key) {
    Entry *e = find_entry(key);
    if (e) printf("%s\n", e->value);
}

void write_all(void) {
    for (Entry *e = head; e; e = e->next)
        printf("%s=%s\n", e->key, e->value);
}

void free_all(void) {
    Entry *e = head;
    while (e) {
        Entry *next = e->next;
        free(e->key);
        free(e->value);
        free(e);
        e = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <configfile>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, f) != -1) {
            line[strcspn(line, "\n")] = '\0';
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *key = line;
                char *value = eq + 1;
                set_entry(key, value);
            }
        }
        free(line);
        fclose(f);
    } else {
        perror("fopen");
        return 1;
    }

    // Commands from stdin
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "SET") == 0 && n == 3) {
            set_entry(arg1, arg2);
        } else if (strcmp(cmd, "UNSET") == 0 && n == 2) {
            unset_entry(arg1);
        } else if (strcmp(cmd, "GET") == 0 && n == 2) {
            get_entry(arg1);
        } else if (strcmp(cmd, "WRITE") == 0 && n == 1) {
            write_all();
        }
    }
    free(line);
    free_all();
    return 0;
}