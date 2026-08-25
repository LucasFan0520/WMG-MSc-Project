// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *name;
    char *note;
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

void add_note(const char *name, const char *note) {
    if (find_note(name)) return;
    Note *n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->note = strdup(note);
    n->next = store;
    store = n;
}

void delete_note(const char *name) {
    Note *prev = NULL;
    Note *n = store;
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
        printf("%s:", n->name);
        for (char *s = n->note; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        n = n->next;
    }
}

void free_store(Note *list) {
    Note *n = list;
    while (n) {
        Note *next = n->next;
        free(n->name);
        free(n->note);
        free(n);
        n = next;
    }
}

int main(int argc, char *argv[]) {
    Note *backup = NULL;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc < 2) continue;
            FILE *f = fopen(argv[1], "r");
            if (!f) continue;
            // backup current store
            backup = store;
            store = NULL;
            int valid = 1;
            char fline[4096];
            while (fgets(fline, sizeof(fline), f)) {
                fline[strcspn(fline, "\n")] = '\0';
                char name[256], note[4096];
                if (sscanf(fline, "%255s %[^\n]", name, note) != 2) {
                    valid = 0;
                    break;
                }
                add_note(name, note);
            }
            fclose(f);
            if (!valid) {
                // restore backup
                free_store(store);
                store = backup;
                backup = NULL;
            } else {
                // free backup
                free_store(backup);
                backup = NULL;
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char name[256], note[4096];
            if (sscanf(rest, "%255s %[^\n]", name, note) == 2) {
                add_note(name, note);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_note(rest);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }
    free_store(store);
    return 0;
}