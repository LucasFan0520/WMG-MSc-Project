// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct note {
    char *name;
    char *text;
    struct note *next;
} Note;

Note *notes = NULL;

Note *find_note(const char *name) {
    Note *n = notes;
    while (n) {
        if (strcmp(n->name, name) == 0) return n;
        n = n->next;
    }
    return NULL;
}

void add_note(const char *name, const char *text) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_note(const char *name) {
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
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
    Note *n = notes;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *new_notes = NULL;
    char line[4096];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], text[4096];
        if (sscanf(line, "%255s %[^\n]", name, text) == 2) {
            Note *n = malloc(sizeof(Note));
            n->name = strdup(name);
            n->text = strdup(text);
            n->next = new_notes;
            new_notes = n;
        } else {
            valid = 0;
            break;
        }
    }
    fclose(f);
    if (valid) {
        // replace old notes with new_notes
        while (notes) {
            Note *tmp = notes;
            notes = notes->next;
            free(tmp->name);
            free(tmp->text);
            free(tmp);
        }
        notes = new_notes;
    } else {
        // free new_notes
        while (new_notes) {
            Note *tmp = new_notes;
            new_notes = new_notes->next;
            free(tmp->name);
            free(tmp->text);
            free(tmp);
        }
    }
}

int main(int argc, char **argv) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_note(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    return 0;
}