// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Note {
    char *category;
    char *text;
    struct Note *next;
} Note;

Note *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *cat = get_token(&ptr);
            if (cat) {
                Note *n = malloc(sizeof(Note));
                if (n) {
                    n->category = strdup(cat);
                    n->text = strdup(ptr);
                    n->next = head;
                    head = n;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = get_token(&ptr);
            if (cat) {
                Note *prev = NULL, *curr = head;
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
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = get_token(&ptr);
            if (cat) {
                Note *prev = NULL, *curr = head;
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
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = get_token(&ptr);
            if (word) {
                for (Note *n = head; n; n = n->next) {
                    if (strstr(n->text, word)) {
                        printf("%s\n", n->text);
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Note *n = head; n; n = n->next) {
                printf("%s %s\n", n->category, n->text);
            }
        }
    }
    while (head) {
        Note *next = head->next;
        free(head->category);
        free(head->text);
        free(head);
        head = next;
    }
    return 0;
}