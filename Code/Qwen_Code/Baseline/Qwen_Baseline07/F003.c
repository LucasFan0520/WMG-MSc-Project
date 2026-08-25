// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Alias {
    char *name, *exp;
    struct Alias *next;
} Alias;

Alias *head = NULL, *tail = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

void print_under(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int main() {
    char cmd[20], n1[256], n2[256], text[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "DEFINE") == 0) {
            scanf("%s", n1); read_rest(text);
            Alias *a = malloc(sizeof(Alias));
            a->name = strdup(n1); a->exp = strdup(text); a->next = NULL;
            if (!head) head = tail = a; else { tail->next = a; tail = a; }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            scanf("%s", n1);
            for (Alias *a = head; a; a = a->next)
                if (strcmp(a->name, n1) == 0) { print_under(a->exp); break; }
        } else if (strcmp(cmd, "RENAME") == 0) {
            scanf("%s %s", n1, n2);
            for (Alias *a = head; a; a = a->next)
                if (strcmp(a->name, n1) == 0) { free(a->name); a->name = strdup(n2); break; }
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%s", n1);
            Alias *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, n1) == 0) {
                    Alias *next = curr->next;
                    if (prev) prev->next = next; else head = next;
                    if (!next) tail = prev;
                    free(curr->name); free(curr->exp); free(curr);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Alias *a = head; a; a = a->next) printf("%s %s\n", a->name, a->exp);
        }
    }
    return 0;
}