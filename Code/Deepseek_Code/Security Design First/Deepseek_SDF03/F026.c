// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_CAT 64
#define MAX_TEXT 4096

typedef struct Note {
    char category[MAX_CAT];
    char text[MAX_TEXT];
    struct Note *next;
} Note;

Note *head = NULL;

void free_all() {
    Note *n = head;
    while (n) {
        Note *tmp = n;
        n = n->next;
        free(tmp);
    }
    head = NULL;
}

void handle_add(char *line) {
    char cat[MAX_CAT], text[MAX_TEXT];
    int n = sscanf(line, "ADD %63s %4095[^\n]", cat, text);
    if (n < 2) return;
    Note *new = malloc(sizeof(Note));
    if (!new) return;
    strcpy(new->category, cat);
    strcpy(new->text, text);
    new->next = head;
    head = new;
}

void handle_delete(char *line) {
    char cat[MAX_CAT];
    if (sscanf(line, "DELETE %63s", cat) != 1) return;
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n);
            n = prev ? prev->next : head;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void handle_keep(char *line) {
    char cat[MAX_CAT];
    if (sscanf(line, "KEEP %63s", cat) != 1) return;
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) != 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n);
            n = prev ? prev->next : head;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void handle_find(char *line) {
    char word[MAX_TEXT];
    if (sscanf(line, "FIND %4095[^\n]", word) != 1) return;
    Note *n = head;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s %s\n", n->category, n->text);
        }
        n = n->next;
    }
}

void handle_print() {
    Note *n = head;
    while (n) {
        printf("%s %s\n", n->category, n->text);
        n = n->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "KEEP ", 5) == 0) handle_keep(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "PRINT") == 0) handle_print();
    }
    free_all();
    return 0;
}