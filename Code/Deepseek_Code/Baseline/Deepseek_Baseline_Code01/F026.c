// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct note {
    char *category;
    char *text;
    struct note *next;
} Note;

Note *head = NULL;
Note *tail = NULL;

void add_note(const char *cat, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->category = strdup(cat);
    n->text = strdup(text);
    n->next = NULL;
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

void delete_category(const char *cat) {
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            if (n == tail) tail = prev;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : head;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void keep_category(const char *cat) {
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) != 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            if (n == tail) tail = prev;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : head;
            continue;
        }
        prev = n;
        n = n->next;
    }
}

void find_word(const char *word) {
    Note *n = head;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s %s\n", n->category, n->text);
        }
        n = n->next;
    }
}

void print_notes(void) {
    Note *n = head;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_category(arg1);
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                keep_category(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %[^\n]", arg1) == 1)
                find_word(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    return 0;
}