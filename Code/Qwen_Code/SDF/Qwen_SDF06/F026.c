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
    if (!n) return;
    n->category = strdup(cat);
    n->text = strdup(text);
    if (!n->category || !n->text) { free(n->category); free(n->text); free(n); return; }
    n->next = head;
    head = n;
}

void delete_cat(const char *cat) {
    Note *curr = head;
    Note *prev = NULL;
    while (curr) {
        if (strcmp(curr->category, cat) == 0) {
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

void keep_cat(const char *cat) {
    Note *curr = head;
    Note *prev = NULL;
    while (curr) {
        if (strcmp(curr->category, cat) != 0) {
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

void find_word(const char *word) {
    for (Note *n = head; n; n = n->next) {
        if (strstr(n->text, word)) {
            printf("%s\n", n->text);
        }
    }
}

void print_all() {
    for (Note *n = head; n; n = n->next) {
        printf("%s %s\n", n->category, n->text);
    }
}

void free_all() {
    Note *n = head;
    while (n) {
        Note *next = n->next;
        free(n->category);
        free(n->text);
        free(n);
        n = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_cat(arg1);
        } else if (strcmp(cmd, "KEEP") == 0 && n >= 2) {
            keep_cat(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_word(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}