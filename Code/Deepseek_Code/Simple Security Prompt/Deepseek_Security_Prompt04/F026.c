/* F026.c */
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
    Note *cur = head;
    while (cur) {
        if (strcmp(cur->category, category) == 0) {
            Note *next = cur->next;
            if (prev) prev->next = next;
            else head = next;
            free(cur->category);
            free(cur->text);
            free(cur);
            cur = next;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void keep_category(char *category) {
    Note *prev = NULL;
    Note *cur = head;
    while (cur) {
        if (strcmp(cur->category, category) != 0) {
            Note *next = cur->next;
            if (prev) prev->next = next;
            else head = next;
            free(cur->category);
            free(cur->text);
            free(cur);
            cur = next;
        } else {
            prev = cur;
            cur = cur->next;
        }
    }
}

void find_word(char *word) {
    for (Note *n = head; n; n = n->next) {
        if (strstr(n->text, word)) {
            printf("%s %s\n", n->category, n->text);
        }
    }
}

void print_all(void) {
    for (Note *n = head; n; n = n->next)
        printf("%s %s\n", n->category, n->text);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_note(arg1, arg2);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_category(arg1);
        } else if (strcmp(cmd, "KEEP") == 0 && n == 2) {
            keep_category(arg1);
        } else if (strcmp(cmd, "FIND") == 0 && n == 2) {
            find_word(arg1);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_all();
        }
    }
    free(line);
    free_all();
    return 0;
}