/*
 * F034.c
 */
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
    Note *prev = NULL, *n = notes;
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

void load_from_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    // store backup of current notes
    Note *backup = notes;
    notes = NULL;
    char line[4096];
    int ok = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256], text[4096];
        if (sscanf(line, "%255s", name) == 1) {
            char *rest = line + strlen(name) + 1;
            while (*rest == ' ') rest++;
            add_note(name, rest);
        } else {
            ok = 0;
            break;
        }
    }
    fclose(f);
    if (!ok) {
        // revert: delete new notes and restore backup
        Note *n = notes;
        while (n) {
            Note *tmp = n;
            n = n->next;
            free(tmp->name);
            free(tmp->text);
            free(tmp);
        }
        notes = backup;
    } else {
        // discard backup
        Note *n = backup;
        while (n) {
            Note *tmp = n;
            n = n->next;
            free(tmp->name);
            free(tmp->text);
            free(tmp);
        }
    }
}

int main(int argc, char **argv) {
    char *filepath = NULL;
    if (argc > 1) filepath = argv[1];
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256];
        if (sscanf(line, "%31s %255s", cmd, name) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(name) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "LOAD") == 0) {
            if (filepath) load_from_file(filepath);
        } else if (strcmp(cmd, "ADD") == 0) {
            add_note(name, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    return 0;
}