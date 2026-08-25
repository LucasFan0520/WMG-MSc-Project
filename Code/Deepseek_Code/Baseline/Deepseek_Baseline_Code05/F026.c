/* F026.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct note {
    char *category;
    char *text;
    struct note *next;
} Note;

Note *notes = NULL;

void add_note(const char *cat, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->category = strdup(cat);
    n->text = strdup(text);
    n->next = notes;
    notes = n;
}

void delete_category(const char *cat) {
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) == 0) {
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

void keep_category(const char *cat) {
    Note *n = notes, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) != 0) {
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
        if (strstr(n->text, word)) {
            for (char *c = n->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
        }
        n = n->next;
    }
}

void print_notes() {
    Note *n = notes;
    while (n) {
        printf("%s ", n->category);
        for (char *c = n->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        n = n->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], cat[256], word[256], text[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PRINT") == 0) { print_notes(); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", cat) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(cat) + 1;
            if (*rest == ' ') rest++;
            add_note(cat, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", cat) == 1) delete_category(cat);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (sscanf(line, "%*s %255s", cat) == 1) keep_category(cat);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", word) == 1) find_word(word);
        }
    }
    return 0;
}