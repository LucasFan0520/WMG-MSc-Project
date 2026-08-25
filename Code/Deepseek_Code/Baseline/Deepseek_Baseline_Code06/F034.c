/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
} Note;

Note *store = NULL;

Note* find_note(const char *name) {
    Note *n = store;
    while (n) {
        if (strcmp(n->name, name) == 0) return n;
        n = n->next;
    }
    return NULL;
}

void add_note(const char *name, const char *note) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = store;
    store = n;
}

void delete_note(const char *name) {
    Note *n = store, *prev = NULL;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else store = n->next;
            free(n->name);
            free(n->note);
            free(n);
            return;
        }
        prev = n;
        n = n->next;
    }
}

void report_notes() {
    Note *n = store;
    while (n) {
        printf("%s %s\n", n->name, n->note);
        n = n->next;
    }
}

void clear_store() {
    while (store) {
        Note *tmp = store;
        store = store->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int load_file(const char *filename, Note **new_store) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    char line[8192];
    int ok = 1;
    Note *list = NULL;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], note[4096];
        if (sscanf(line, "%255s %[^\n]", name, note) == 2) {
            Note *n = malloc(sizeof(Note));
            n->name = strdup(name);
            n->note = strdup(note);
            n->next = list;
            list = n;
        } else {
            ok = 0;
            break;
        }
    }
    fclose(f);
    if (ok) {
        *new_store = list;
        return 1;
    } else {
        while (list) {
            Note *tmp = list;
            list = list->next;
            free(tmp->name);
            free(tmp->note);
            free(tmp);
        }
        return 0;
    }
}

void free_all() {
    clear_store();
}

int main(int argc, char **argv) {
    char *filename = (argc > 1) ? argv[1] : NULL;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], note[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, name, note);
        if (strcmp(cmd, "LOAD") == 0 && n == 1) {
            if (filename) {
                Note *new_store = NULL;
                if (load_file(filename, &new_store)) {
                    clear_store();
                    store = new_store;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_note(name, note);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_notes();
        }
    }
    free_all();
    return 0;
}