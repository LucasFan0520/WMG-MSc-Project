// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Note {
    char *category, *text;
    struct Note *next;
} Note;

Note *head = NULL;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

int main() {
    char cmd[20], cat[256], text[1024], word[256];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", cat); read_rest(text, sizeof(text));
            Note *n = malloc(sizeof(Note));
            n->category = strdup(cat); n->text = strdup(text); n->next = head; head = n;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", cat);
            Note *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->category, cat) == 0) {
                    Note *next = curr->next;
                    if (prev) prev->next = next; else head = next;
                    free(curr->category); free(curr->text); free(curr);
                    curr = next;
                } else {
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            scanf("%s", cat);
            Note *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->category, cat) != 0) {
                    Note *next = curr->next;
                    if (prev) prev->next = next; else head = next;
                    free(curr->category); free(curr->text); free(curr);
                    curr = next;
                } else {
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", word);
            for (Note *n = head; n; n = n->next) {
                if (strstr(n->text, word)) printf("%s\n", n->text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Note *n = head; n; n = n->next) printf("%s %s\n", n->category, n->text);
        }
    }
    return 0;
}