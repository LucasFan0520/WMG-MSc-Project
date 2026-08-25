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

void free_store(void) {
    Note *n = store;
    while (n) {
        Note *next = n->next;
        free(n->name);
        free(n->note);
        free(n);
        n = next;
    }
    store = NULL;
}

void load_from_file(char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    // build new list temporarily
    Note *new_store = NULL;
    Note *tail = NULL;
    char *line = NULL;
    size_t len = 0;
    int ok = 1;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], note[1024];
        int n = sscanf(line, "%255s %[^\n]", name, note);
        if (n != 2) { ok = 0; break; }
        Note *nn = malloc(sizeof(Note));
        nn->name = strdup(name);
        nn->note = strdup(note);
        nn->next = NULL;
        if (!tail) new_store = tail = nn;
        else { tail->next = nn; tail = nn; }
    }
    free(line);
    fclose(f);
    if (ok) {
        // replace store
        free_store();
        store = new_store;
    } else {
        // free new_store
        Note *n = new_store;
        while (n) {
            Note *next = n->next;
            free(n->name);
            free(n->note);
            free(n);
            n = next;
        }
    }
}

void add_note(char *name, char *note) {
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = store;
    store = n;
}

void delete_note(char *name) {
    Note *prev = NULL;
    Note *cur = store;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else store = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void report_all(void) {
    for (Note *n = store; n; n = n->next)
        printf("%s %s\n", n->name, n->note);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }
    char *filename = argv[1];
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "LOAD") == 0 && n == 1) {
            load_from_file(filename);
        } else if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_note(arg1);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_store();
    return 0;
}