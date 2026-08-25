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

void add_note(char *category, char *text) {
    Note *n = malloc(sizeof(Note));
    n->category = strdup(category);
    n->text = strdup(text);
    n->next = head;
    head = n;
}

void delete_category(char *category) {
    Note *prev = NULL;
    Note *curr = head;
    while (curr) {
        if (strcmp(curr->category, category) == 0) {
            Note *next = curr->next;
            if (prev) prev->next = next;
            else head = next;
            free(curr->category);
            free(curr->text);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void keep_category(char *category) {
    Note *prev = NULL;
    Note *curr = head;
    while (curr) {
        if (strcmp(curr->category, category) != 0) {
            Note *next = curr->next;
            if (prev) prev->next = next;
            else head = next;
            free(curr->category);
            free(curr->text);
            free(curr);
            curr = next;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }
}

void find_word(char *word) {
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
    char cmd[20], a[100], text[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s", a);
            fgets(text, sizeof(text), stdin);
            while (text[0] == ' ') memmove(text, text + 1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            add_note(a, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_category(a);
        } else if (strcmp(cmd, "KEEP") == 0) {
            scanf("%99s", a);
            keep_category(a);
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%99s", a);
            find_word(a);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    return 0;
}