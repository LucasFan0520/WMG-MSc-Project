// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note { char *category; char *text; struct Note *next; } Note;
Note *notes = NULL;

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    if (!n) return;
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_category(const char *category) {
    Note *prev = NULL;
    for (Note *n = notes; n; prev = n, n = n->next) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next; else notes = n->next;
            free(n->category); free(n->text); free(n);
            n = prev ? prev : notes; // continue
        }
    }
}

void keep_category(const char *category) {
    Note *prev = NULL;
    Note *cur = notes;
    while (cur) {
        if (strcmp(cur->category, category) != 0) {
            Note *to_delete = cur;
            if (prev) prev->next = cur->next; else notes = cur->next;
            cur = cur->next;
            free(to_delete->category); free(to_delete->text); free(to_delete);
        } else {
            prev = cur;
            cur = cur->next;
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

void print_notes(void) {
    for (Note *n = notes; n; n = n->next)
        printf("%s %s\n", n->category, n->text);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char category[256], text[4096];
            if (sscanf(rest, "%255s", category) == 1) {
                char *t = rest + strlen(category);
                while (*t == ' ') t++;
                strcpy(text, t);
                add_note(category, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char category[256];
            if (sscanf(rest, "%255s", category) == 1) delete_category(category);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char category[256];
            if (sscanf(rest, "%255s", category) == 1) keep_category(category);
        } else if (strcmp(cmd, "FIND") == 0) {
            char word[256];
            if (sscanf(rest, "%255s", word) == 1) find_word(word);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    while (notes) { Note *n = notes; notes = n->next; free(n->category); free(n->text); free(n); }
    return 0;
}