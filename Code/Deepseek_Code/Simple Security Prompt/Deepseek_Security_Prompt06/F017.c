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

Entry *find_entry(const char *key) {
    Entry *e = head;
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
        e->value = malloc(strlen(value) + 1);
        strcpy(e->value, value);
        return;
    }
    Entry *new = malloc(sizeof(Entry));
    new->key = malloc(strlen(key) + 1);
    strcpy(new->key, key);
    new->value = malloc(strlen(value) + 1);
    strcpy(new->value, value);
    new->next = head;
    head = new;
}

void unset_entry(const char *key) {
    Entry *e = head, *prev = NULL;
    while (e) {
        if (strcmp(e->key, key) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
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
    if (e) printf("%s\n", e->value);
}

void write_all(void) {
    Entry *e = head;
    while (e) {
        printf("%s=%s\n", e->key, e->value);
        e = e->next;
    }
}

void load_file(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (!fp) return;
    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        set_entry(key, value);
    }
    fclose(fp);
}

void free_all(void) {
    Entry *e = head;
    while (e) {
        Entry *tmp = e->next;
        free(e->key);
        free(e->value);
        free(e);
        e = tmp;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(rest, "%255s", key) == 1) {
                const char *val = rest + strlen(key) + 1;
                while (*val == ' ') val++;
                if (*val) set_entry(key, val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(rest, "%255s", key) == 1) unset_entry(key);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(rest, "%255s", key) == 1) get_entry(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_all();
        }
    }
    free_all();
    return 0;
}