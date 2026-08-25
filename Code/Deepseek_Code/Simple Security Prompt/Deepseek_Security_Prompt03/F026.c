/*
 * F026.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct note {
    char *category;
    char *text;
    struct note *next;
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
    Note *prev = NULL, *n = notes;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            if (prev) n = prev->next;
            else n = notes;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void keep_category(const char *category) {
    Note *prev = NULL, *n = notes;
    while (n) {
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = n->next;
            else notes = n->next;
            free(n->category);
            free(n->text);
            free(n);
            if (prev) n = prev->next;
            else n = notes;
        } else {
            prev = n;
            n = n->next;
        }
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

void print_notes(void) {
    Note *n = notes;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256];
        if (sscanf(line, "%31s %255s", cmd, arg1) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            add_note(arg1, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_category(arg1);
        } else if (strcmp(cmd, "KEEP") == 0) {
            keep_category(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_word(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    return 0;
}