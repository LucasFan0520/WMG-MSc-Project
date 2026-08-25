/* F026.c */
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
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : notes;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void keep_category(const char *category) {
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : notes;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void find_word(const char *word) {
    Note *n = notes;
    while (n) {
        if (strstr(n->text, word) != NULL) {
            printf("%s %s\n", n->category, n->text);
        }
        n = n->next;
    }
}

void print_notes() {
    Note *n = notes;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

void free_all() {
    while (notes) {
        Note *tmp = notes;
        notes = notes->next;
        free(tmp->category);
        free(tmp->text);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, a, b);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_note(a, b);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_category(a);
        } else if (strcmp(cmd, "KEEP") == 0 && n >= 2) {
            keep_category(a);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_word(a);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_notes();
        }
    }
    free_all();
    return 0;
}