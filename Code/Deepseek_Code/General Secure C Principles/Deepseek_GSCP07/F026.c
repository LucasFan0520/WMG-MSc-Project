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

void free_note(Note *n) {
    free(n->category);
    free(n->text);
    free(n);
}

void handle_add(char *cmd) {
    char *cat = strtok(NULL, " ");
    if (!cat) return;
    char *text = strtok(NULL, "");
    if (!text) text = "";
    Note *new = malloc(sizeof(Note));
    new->category = strdup(cat);
    new->text = strdup(text);
    new->next = NULL;
    if (!head) head = new;
    else {
        Note *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_delete(char *cmd) {
    char *cat = strtok(NULL, " ");
    if (!cat) return;
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free_note(n);
            n = prev ? prev->next : head;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void handle_keep(char *cmd) {
    char *cat = strtok(NULL, " ");
    if (!cat) return;
    Note *n = head, *prev = NULL;
    while (n) {
        if (strcmp(n->category, cat) != 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free_note(n);
            n = prev ? prev->next : head;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void handle_find(char *cmd) {
    char *word = strtok(NULL, " ");
    if (!word) return;
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) handle_add(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "KEEP") == 0) handle_keep(cmd);
        else if (strcmp(cmd, "FIND") == 0) handle_find(cmd);
        else if (strcmp(cmd, "PRINT") == 0) handle_print();
    }
    while (head) {
        Note *tmp = head;
        head = head->next;
        free_note(tmp);
    }
    return 0;
}