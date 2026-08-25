/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
    struct Note *next;
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

void add_note(const char *name, const char *note) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = notes;
    notes = n;
}

void delete_note(const char *name) {
    Note *n = notes, *prev = NULL;
    while (n && strcmp(n->name, name) != 0) {
        prev = n;
        n = n->next;
    }
    if (!n) return;
    if (prev) prev->next = n->next;
    else notes = n->next;
    free(n->name);
    free(n->note);
    free(n);
}

void report_notes(void) {
    Note *n = notes;
    while (n) {
        printf("%s ", n->name);
        for (char *c = n->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        n = n->next;
    }
}

void load_notes(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    Note *new_notes = NULL;
    char line[1024];
    int valid = 1;
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256];
        char *p = line;
        if (sscanf(p, "%255s", name) != 1) { valid = 0; break; }
        while (*p && *p != ' ') p++;
        while (*p == ' ') p++;
        Note *n = malloc(sizeof(Note));
        n->name = strdup(name);
        n->note = strdup(p);
        n->next = new_notes;
        new_notes = n;
    }
    fclose(f);
    if (valid) {
        Note *n = notes;
        while (n) {
            Note *next = n->next;
            free(n->name);
            free(n->note);
            free(n);
            n = next;
        }
        notes = new_notes;
    } else {
        Note *n = new_notes;
        while (n) {
            Note *next = n->next;
            free(n->name);
            free(n->note);
            free(n);
            n = next;
        }
    }
}

void free_all(void) {
    Note *n = notes;
    while (n) {
        Note *next = n->next;
        free(n->name);
        free(n->note);
        free(n);
        n = next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    char *filename = argv[1];
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            load_notes(filename);
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            char *p = line + 4;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_note(name, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_note(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_all();
    free(line);
    return 0;
}