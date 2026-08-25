/* F034.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_note(Note *n) {
    free(n->name);
    free(n->text);
    free(n);
}

void free_all_notes(void) {
    while (notes) {
        Note *tmp = notes;
        notes = notes->next;
        free_note(tmp);
    }
}

void add_note(const char *name, const char *text) {
    Note *n = find_note(name);
    if (n) {
        free(n->text);
        n->text = strdup(text);
        return;
    }
    n = malloc(sizeof(Note));
    n->name = strdup(name);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_note(const char *name) {
    Note *prev = NULL, *curr = notes;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else notes = curr->next;
            free_note(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void report_notes(void) {
    Note *n = notes;
    while (n) {
        printf("%s %s\n", n->name, n->text);
        n = n->next;
    }
}

int load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    char line[4096];
    Note *new_notes = NULL;
    int ok = 1;
    while (fgets(line, sizeof(line), f)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        char name[256];
        if (sscanf(line, "%255s", name) != 1) { ok = 0; break; }
        char *p = line + strlen(name);
        while (*p == ' ') p++;
        Note *n = malloc(sizeof(Note));
        n->name = strdup(name);
        n->text = strdup(p);
        n->next = new_notes;
        new_notes = n;
    }
    fclose(f);
    if (ok) {
        free_all_notes();
        notes = new_notes;
        return 1;
    } else {
        while (new_notes) {
            Note *tmp = new_notes;
            new_notes = new_notes->next;
            free_note(tmp);
        }
        return 0;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(int argc, char *argv[]) {
    char line[4096];
    char cmd[32];
    char name[256];
    char text[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "LOAD") == 0) {
            if (argc > 1) load_file(argv[1]);
        } else if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            add_note(name, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) delete_note(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_notes();
        }
    }

    free_all_notes();
    return 0;
}