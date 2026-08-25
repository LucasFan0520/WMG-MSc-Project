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
    if (!n) return;
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_note(const char *name) {
    Note *prev = NULL, *cur = notes;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else notes = cur->next;
            free(cur->name);
            free(cur->text);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void report_notes(void) {
    Note *n = notes;
    while (n) {
        printf("%s: ", n->name);
        char *s = n->text;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        n = n->next;
    }
}

void free_notes(Note *list) {
    while (list) {
        Note *next = list->next;
        free(list->name);
        free(list->text);
        free(list);
        list = next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing file argument\n");
        return 1;
    }
    char *filename = argv[1];
    Note *saved_notes = NULL; // for rollback

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "LOAD") == 0) {
            // Save current notes for rollback
            saved_notes = notes;
            notes = NULL;
            FILE *f = fopen(filename, "r");
            if (!f) {
                // restore
                notes = saved_notes;
                saved_notes = NULL;
                continue;
            }
            int valid = 1;
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char *space = strchr(line, ' ');
                if (!space) {
                    valid = 0;
                    break;
                }
                *space = '\0';
                char *name = line;
                char *text = space + 1;
                if (find_note(name)) { valid = 0; break; }
                add_note(name, text);
            }
            fclose(f);
            if (!valid) {
                // rollback: free new notes and restore saved
                free_notes(notes);
                notes = saved_notes;
            } else {
                // success: discard saved notes
                free_notes(saved_notes);
            }
            saved_notes = NULL;
        } else if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *text = space + 1;
            while (*text == ' ') text++;
            add_note(name, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_notes(notes);
    free_notes(saved_notes);
    return 0;
}