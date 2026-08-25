// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Note {
    char *name;
    char *text;
    struct Note *next;
} Note;

Note *notes = NULL;

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    Note *new_notes = NULL;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256];
        int n = 0;
        if (sscanf(line, "%255s%n", name, &n) != 1) continue;
        char *text = line + n;
        while (*text == ' ') text++;
        Note *note = malloc(sizeof(Note));
        if (!note) continue;
        note->name = strdup(name);
        note->text = strdup(text);
        if (!note->name || !note->text) {
            free(note->name); free(note->text); free(note); continue;
        }
        note->next = new_notes;
        new_notes = note;
    }
    fclose(f);
    while (notes) {
        Note *n = notes;
        notes = notes->next;
        free(n->name); free(n->text); free(n);
    }
    notes = new_notes;
}

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", name, &n) != 1) continue;
            char *text = rest + n;
            while (*text == ' ') text++;
            Note *note = malloc(sizeof(Note));
            if (!note) continue;
            note->name = strdup(name);
            note->text = strdup(text);
            if (!note->name || !note->text) {
                free(note->name); free(note->text); free(note); continue;
            }
            note->next = notes;
            notes = note;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Note *prev = NULL;
            for (Note *n = notes; n; prev = n, n = n->next) {
                if (strcmp(n->name, name) == 0) {
                    if (prev) prev->next = n->next;
                    else notes = n->next;
                    free(n->name); free(n->text); free(n);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Note *n = notes; n; n = n->next) {
                printf("%s ", n->name);
                print_text(n->text);
            }
        }
    }
    while (notes) {
        Note *n = notes;
        notes = notes->next;
        free(n->name); free(n->text); free(n);
    }
    return 0;
}