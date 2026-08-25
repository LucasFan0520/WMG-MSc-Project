// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
} Note;

Note *notes = NULL;

void add_note(const char *name, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_note(const char *name) {
    Note **np = &notes;
    while (*np) {
        if (strcmp((*np)->name, name) == 0) {
            Note *tmp = *np;
            *np = tmp->next;
            free(tmp->name);
            free(tmp->text);
            free(tmp);
            return;
        }
        np = &(*np)->next;
    }
}

void report() {
    for (Note *n = notes; n; n = n->next) {
        printf("%s %s\n", n->name, n->text);
    }
}

void cleanup_notes() {
    while (notes) {
        Note *tmp = notes->next;
        free(notes->name);
        free(notes->text);
        free(notes);
        notes = tmp;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *new_notes = NULL;
    char line[8192];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256];
        if (sscanf(line, "%255s", name) != 1) { valid = 0; break; }
        char *t = strchr(line, ' ');
        if (!t) { valid = 0; break; }
        Note *n = malloc(sizeof(Note));
        n->name = strdup(name);
        n->text = strdup(t + 1);
        n->next = new_notes;
        new_notes = n;
    }
    fclose(f);
    if (valid) {
        cleanup_notes();
        notes = new_notes;
    } else {
        while (new_notes) {
            Note *tmp = new_notes->next;
            free(new_notes->name);
            free(new_notes->text);
            free(new_notes);
            new_notes = tmp;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) add_note(a1, t + 1); }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_note(a1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    cleanup_notes();
    return 0;
}