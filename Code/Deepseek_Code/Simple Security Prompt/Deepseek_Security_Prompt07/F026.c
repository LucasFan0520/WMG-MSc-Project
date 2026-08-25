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
    Note *prev = NULL;
    Note *n = notes;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : notes;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void keep_category(const char *category) {
    Note *prev = NULL;
    Note *n = notes;
    while (n) {
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : notes;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void find_word(const char *word) {
    Note *n = notes;
    int first = 1;
    while (n) {
        if (strstr(n->text, word)) {
            if (!first) putchar(' ');
            printf("%s", n->category);
            first = 0;
        }
        n = n->next;
    }
    putchar('\n');
}

void print_notes() {
    Note *n = notes;
    while (n) {
        printf("%s:", n->category);
        for (char *s = n->text; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        n = n->next;
    }
}

void free_all() {
    Note *n = notes;
    while (n) {
        Note *next = n->next;
        free(n->category);
        free(n->text);
        free(n);
        n = next;
    }
}

int main() {
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
        if (strcmp(cmd, "ADD") == 0) {
            char cat[256], text[4096];
            if (sscanf(rest, "%255s %[^\n]", cat, text) == 2) {
                add_note(cat, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_category(rest);
        } else if (strcmp(cmd, "KEEP") == 0) {
            keep_category(rest);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_word(rest);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free_all();
    return 0;
}