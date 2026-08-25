// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *head = NULL;

void add_note(const char *cat, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->category = strdup(cat);
    n->text = strdup(text);
    n->next = head;
    head = n;
}

void delete_cat(const char *cat) {
    Note *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->category, cat) == 0) {
            Note *next = p->next;
            if (prev) prev->next = next;
            else head = next;
            free(p->category); free(p->text); free(p);
            p = next;
        } else {
            prev = p; p = p->next;
        }
    }
}

void keep_cat(const char *cat) {
    Note *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->category, cat) != 0) {
            Note *next = p->next;
            if (prev) prev->next = next;
            else head = next;
            free(p->category); free(p->text); free(p);
            p = next;
        } else {
            prev = p; p = p->next;
        }
    }
}

void find_word(const char *word) {
    for (Note *n = head; n; n = n->next) {
        if (strstr(n->text, word)) {
            printf("%s\n", n->text);
        }
    }
}

void print_notes() {
    for (Note *n = head; n; n = n->next) {
        printf("%s %s\n", n->category, n->text);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            add_note(a, b);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_cat(a);
        } else if (strcmp(cmd, "KEEP") == 0) {
            sscanf(line, "%*s %s", a);
            keep_cat(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            find_word(a);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    while (head) {
        Note *n = head->next;
        free(head->category); free(head->text); free(head);
        head = n;
    }
    return 0;
}