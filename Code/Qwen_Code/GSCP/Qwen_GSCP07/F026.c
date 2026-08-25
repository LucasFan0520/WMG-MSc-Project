// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *notes = NULL;

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_category(const char *category) {
    Note **np = &notes;
    while (*np) {
        if (strcmp((*np)->category, category) == 0) {
            Note *tmp = *np;
            *np = tmp->next;
            free(tmp->category);
            free(tmp->text);
            free(tmp);
        } else {
            np = &(*np)->next;
        }
    }
}

void keep_category(const char *category) {
    Note **np = &notes;
    while (*np) {
        if (strcmp((*np)->category, category) != 0) {
            Note *tmp = *np;
            *np = tmp->next;
            free(tmp->category);
            free(tmp->text);
            free(tmp);
        } else {
            np = &(*np)->next;
        }
    }
}

void find_word(const char *word) {
    for (Note *n = notes; n; n = n->next) {
        if (strstr(n->text, word)) {
            printf("%s\n", n->text);
        }
    }
}

void print_notes() {
    for (Note *n = notes; n; n = n->next) {
        printf("%s %s\n", n->category, n->text);
    }
}

void cleanup() {
    while (notes) {
        Note *tmp = notes->next;
        free(notes->category);
        free(notes->text);
        free(notes);
        notes = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) add_note(a1, t + 1); }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_category(a1);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) keep_category(a1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) find_word(a1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    cleanup();
    return 0;
}