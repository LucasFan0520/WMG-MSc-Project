// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
} Note;

Note *store = NULL;

Note *find_note(const char *name) {
    Note *n = store;
    while (n) {
        if (strcmp(n->name, name) == 0) return n;
        n = n->next;
    }
    return NULL;
}

void add_note(const char *name, const char *text) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = NULL;
    if (!n->name || !n->text) {
        free(n->name); free(n->text); free(n);
        return;
    }
    if (!store) store = n;
    else {
        Note *last = store;
        while (last->next) last = last->next;
        last->next = n;
    }
}

void delete_note(const char *name) {
    Note *n = store, *prev = NULL;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else store = n->next;
            free(n->name);
            free(n->text);
            free(n);
            return;
        }
        prev = n;
        n = n->next;
    }
}

void report_notes(void) {
    Note *n = store;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

void free_store(Note *s) {
    Note *n = s;
    while (n) {
        Note *next = n->next;
        free(n->name);
        free(n->text);
        free(n);
        n = next;
    }
}

void load_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    // save current store as backup
    Note *backup = store;
    store = NULL;
    char *line = NULL;
    size_t len = 0;
    int error = 0;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], text[4096];
        if (sscanf(line, "%255s %4095[^\n]", name, text) == 2) {
            add_note(name, text);
        } else {
            error = 1;
            break;
        }
    }
    free(line);
    fclose(f);
    if (error) {
        // restore backup
        free_store(store);
        store = backup;
    } else {
        // discard backup
        free_store(backup);
    }
}

void free_all(void) {
    free_store(store);
}

int main(int argc, char **argv) {
    char *filename = (argc >= 2) ? argv[1] : NULL;

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], text[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            if (filename) load_from_file(filename);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, text) == 2)
                add_note(name, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free(line);
    free_all();
    return 0;
}