// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct L {
    char *text;
    struct L *next;
} L;

L *head = NULL;
int count = 0;

void insert(int idx, char *text) {
    if (idx < 0 || idx > count) {
        return;
    }
    L *n = malloc(sizeof(L));
    n->text = strdup(text);
    if (idx == 0) {
        n->next = head;
        head = n;
    } else {
        L *p = head;
        for (int i = 0; i < idx - 1; i++) {
            p = p->next;
        }
        n->next = p->next;
        p->next = n;
    }
    count++;
}

int main() {
    char cmd[20];
    char text[1000];
    int idx;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "INSERT")) {
            scanf("%d", &idx);
            fgets(text, 1000, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            insert(idx, text);
        } else if (!strcmp(cmd, "APPEND")) {
            fgets(text, 1000, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            insert(count, text);
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%d", &idx);
            if (idx < 0 || idx >= count) {
                continue;
            }
            L *p = head;
            L *prev = NULL;
            for (int i = 0; i < idx; i++) {
                prev = p;
                p = p->next;
            }
            if (prev) {
                prev->next = p->next;
            } else {
                head = p->next;
            }
            free(p->text);
            free(p);
            count--;
        } else if (!strcmp(cmd, "PATCH")) {
            scanf("%d", &idx);
            fgets(text, 1000, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            if (idx < 0 || idx >= count) {
                continue;
            }
            L *p = head;
            for (int i = 0; i < idx; i++) {
                p = p->next;
            }
            free(p->text);
            p->text = strdup(text);
        } else if (!strcmp(cmd, "PRINT")) {
            for (L *p = head; p; p = p->next) {
                for (char *c = p->text; *c; c++) {
                    if (*c == ' ') {
                        printf("_");
                    } else {
                        printf("%c", *c);
                    }
                }
                printf("\n");
            }
        }
    }
    return 0;
}