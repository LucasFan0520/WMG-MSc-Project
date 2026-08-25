// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct P {
    char *id;
    char *text;
    struct P *next;
} P;

P *head = NULL;
P *tail = NULL;

void add(char *id, char *text) {
    P *n = malloc(sizeof(P));
    n->id = strdup(id);
    n->text = strdup(text);
    n->next = NULL;
    if (!head) {
        head = tail = n;
    } else {
        tail->next = n;
        tail = n;
    }
}

P* find(char *id) {
    for (P *p = head; p; p = p->next) {
        if (!strcmp(p->id, id)) {
            return p;
        }
    }
    return NULL;
}

int main() {
    char cmd[20];
    char id[100];
    char text[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "NEW") || !strcmp(cmd, "APPEND") || !strcmp(cmd, "REPLACE")) {
            scanf("%s", id);
            fgets(text, 1000, stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            if (!strcmp(cmd, "NEW") || !strcmp(cmd, "REPLACE")) {
                P *p = find(id);
                if (p) {
                    free(p->text);
                    p->text = strdup(text);
                } else {
                    add(id, text);
                }
            } else {
                P *p = find(id);
                if (p) {
                    char *nt = malloc(strlen(p->text) + strlen(text) + 2);
                    sprintf(nt, "%s %s", p->text, text);
                    free(p->text);
                    p->text = nt;
                } else {
                    add(id, text);
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", id);
            P *p = head;
            P *prev = NULL;
            while (p) {
                if (!strcmp(p->id, id)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    if (p == tail) {
                        tail = prev;
                    }
                    free(p->id);
                    free(p->text);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            scanf("%s", id);
            P *p = find(id);
            if (!p) {
                printf("NOT_FOUND\n");
            } else {
                for (char *c = p->text; *c; c++) {
                    if (*c == ' ') {
                        printf("_");
                    } else {
                        printf("%c", *c);
                    }
                }
                printf("\n");
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (P *p = head; p; p = p->next) {
                printf("%s\n", p->id);
            }
        }
    }
    return 0;
}