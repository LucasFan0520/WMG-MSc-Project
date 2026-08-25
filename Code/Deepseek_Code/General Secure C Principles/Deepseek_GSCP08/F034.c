// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note { char *name; char *note; struct Note *next; } Note;
Note *store = NULL;

Note *find_note(const char *name) {
    for (Note *n = store; n; n = n->next)
        if (strcmp(n->name, name) == 0) return n;
    return NULL;
}

void add_note(const char *name, const char *note) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = store;
    store = n;
}

void delete_note(const char *name) {
    Note *prev = NULL;
    for (Note *n = store; n; prev = n, n = n->next) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next; else store = n->next;
            free(n->name); free(n->note); free(n);
            return;
        }
    }
}

void report_notes(void) {
    for (Note *n = store; n; n = n->next)
        printf("%s %s\n", n->name, n->note);
}

void clear_store(void) {
    while (store) { Note *n = store; store = n->next; free(n->name); free(n->note); free(n); }
}

int load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    char line[4096];
    Note *new_store = NULL;
    Note *tail = NULL;
    int ok = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256], note[4096];
        if (sscanf(line, "%255s", name) == 1) {
            char *n = line + strlen(name);
            while (*n == ' ') n++;
            strcpy(note, n);
            Note *nn = malloc(sizeof(Note));
            if (!nn) { ok = 0; break; }
            nn->name = strdup(name);
            nn->note = strdup(note);
            nn->next = NULL;
            if (tail) tail->next = nn; else new_store = nn;
            tail = nn;
        }
    }
    fclose(f);
    if (ok) {
        clear_store();
        store = new_store;
        return 1;
    } else {
        while (new_store) { Note *n = new_store; new_store = n->next; free(n->name); free(n->note); free(n); }
        return 0;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]); // initial load, ignore failure
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) {
                load_file(argv[1]);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *n = rest + strlen(name);
                while (*n == ' ') n++;
                strcpy(note, n);
                add_note(name, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    clear_store();
    return 0;
}