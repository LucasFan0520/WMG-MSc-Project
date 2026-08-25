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

int main() {
    char cmd[20];
    char category[100];
    char text[1000];
    char word[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", category);
            fgets(text, 1000, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            Note *n = malloc(sizeof(Note));
            n->category = strdup(category);
            n->text = strdup(text);
            n->next = head;
            head = n;
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", category);
            Note *p = head;
            Note *prev = NULL;
            while (p) {
                if (!strcmp(p->category, category)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    Note *tmp = p;
                    p = p->next;
                    free(tmp->category);
                    free(tmp->text);
                    free(tmp);
                } else {
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "KEEP")) {
            scanf("%s", category);
            Note *p = head;
            Note *prev = NULL;
            while (p) {
                if (strcmp(p->category, category) != 0) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    Note *tmp = p;
                    p = p->next;
                    free(tmp->category);
                    free(tmp->text);
                    free(tmp);
                } else {
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", word);
            for (Note *n = head; n; n = n->next) {
                if (strstr(n->text, word)) {
                    printf("%s\n", n->text);
                }
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (Note *n = head; n; n = n->next) {
                printf("%s %s\n", n->category, n->text);
            }
        }
    }
    return 0;
}