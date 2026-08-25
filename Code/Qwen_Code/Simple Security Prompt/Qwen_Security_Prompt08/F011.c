// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct D {
    char *title;
    char *body;
    struct D *next;
} D;

D *drafts = NULL;
D *trash = NULL;

int main() {
    char cmd[20];
    char title[100];
    char body[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "CREATE") || !strcmp(cmd, "REVISE")) {
            scanf("%s", title);
            fgets(body, 1000, stdin);
            body[strcspn(body, "\n")] = 0;
            if (body[0] == ' ') {
                memmove(body, body + 1, strlen(body));
            }
            D *p = drafts;
            while (p && strcmp(p->title, title)) {
                p = p->next;
            }
            if (p) {
                free(p->body);
                p->body = strdup(body);
            } else {
                D *nd = malloc(sizeof(D));
                nd->title = strdup(title);
                nd->body = strdup(body);
                nd->next = drafts;
                drafts = nd;
            }
        } else if (!strcmp(cmd, "DISCARD")) {
            scanf("%s", title);
            D *p = drafts;
            D *prev = NULL;
            while (p) {
                if (!strcmp(p->title, title)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        drafts = p->next;
                    }
                    p->next = trash;
                    trash = p;
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "RESTORE")) {
            if (trash) {
                D *tmp = trash;
                trash = trash->next;
                tmp->next = drafts;
                drafts = tmp;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", title);
            D *p = drafts;
            while (p && strcmp(p->title, title)) {
                p = p->next;
            }
            if (p) {
                printf("%s\n", p->body);
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (D *p = drafts; p; p = p->next) {
                printf("%s %s\n", p->title, p->body);
            }
        }
    }
    return 0;
}