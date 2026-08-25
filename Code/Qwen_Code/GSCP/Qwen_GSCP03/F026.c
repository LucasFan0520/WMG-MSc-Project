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

void free_list() {
    while (head) {
        Note *n = head; head = head->next;
        free(n->category); free(n->text); free(n);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], cat[256], text[4096] = "", word[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", cat, text) < 1) continue;
            Note *n = malloc(sizeof(Note));
            n->category = strdup(cat); n->text = strdup(text);
            n->next = head; head = n;
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", cat) != 1) continue;
            Note *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->category, cat) == 0) {
                    Note *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->category); free(tmp->text); free(tmp);
                } else {
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            if (sscanf(line, "%*s %255s", cat) != 1) continue;
            Note *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->category, cat) != 0) {
                    Note *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->category); free(tmp->text); free(tmp);
                } else {
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", word) != 1) continue;
            for (Note *n = head; n; n = n->next) {
                if (strstr(n->text, word)) {
                    printf("%s\n", n->text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Note *n = head; n; n = n->next) {
                printf("%s %s\n", n->category, n->text);
            }
        }
    }
    free_list();
    return 0;
}