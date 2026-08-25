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
Note *tail = NULL;

void add_note(const char *category, const char *text) {
    Note *n = malloc(sizeof(Note));
    n->category = malloc(strlen(category)+1);
    strcpy(n->category, category);
    n->text = malloc(strlen(text)+1);
    strcpy(n->text, text);
    n->next = NULL;
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

void delete_category(const char *category) {
    Note *prev = NULL;
    Note *n = head;
    while (n) {
        if (strcmp(n->category, category) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            if (n == tail) tail = prev;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : head;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void keep_category(const char *category) {
    Note *prev = NULL;
    Note *n = head;
    while (n) {
        if (strcmp(n->category, category) != 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            if (n == tail) tail = prev;
            free(n->category);
            free(n->text);
            free(n);
            n = prev ? prev->next : head;
        } else {
            prev = n;
            n = n->next;
        }
    }
}

void find_word(const char *word) {
    Note *n = head;
    while (n) {
        if (strstr(n->text, word)) {
            printf("%s\n", n->text);
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

void free_all(void) {
    Note *n = head;
    while (n) {
        Note *next = n->next;
        free(n->category);
        free(n->text);
        free(n);
        n = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *category = strtok(rest, " ");
            if (!category) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            add_note(category, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *category = strtok(rest, " ");
            if (category) delete_category(category);
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *category = strtok(rest, " ");
            if (category) keep_category(category);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = strtok(rest, " ");
            if (word) find_word(word);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_notes();
        }
    }
    free_all();
    return 0;
}