// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct A {
    char *name;
    char *exp;
    struct A *next;
} A;

A *head = NULL;

int main() {
    char cmd[20];
    char n1[100];
    char n2[100];
    char exp[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "DEFINE")) {
            scanf("%s", n1);
            fgets(exp, 1000, stdin);
            exp[strcspn(exp, "\n")] = 0;
            if (exp[0] == ' ') {
                memmove(exp, exp + 1, strlen(exp));
            }
            A *p = head;
            while (p && strcmp(p->name, n1)) {
                p = p->next;
            }
            if (p) {
                free(p->exp);
                p->exp = strdup(exp);
            } else {
                A *nn = malloc(sizeof(A));
                nn->name = strdup(n1);
                nn->exp = strdup(exp);
                nn->next = head;
                head = nn;
            }
        } else if (!strcmp(cmd, "EXPAND")) {
            scanf("%s", n1);
            A *p = head;
            while (p && strcmp(p->name, n1)) {
                p = p->next;
            }
            if (p) {
                for (char *c = p->exp; *c; c++) {
                    if (*c == ' ') {
                        printf("_");
                    } else {
                        printf("%c", *c);
                    }
                }
                printf("\n");
            }
        } else if (!strcmp(cmd, "RENAME")) {
            scanf("%s %s", n1, n2);
            A *p = head;
            while (p && strcmp(p->name, n1)) {
                p = p->next;
            }
            if (p) {
                free(p->name);
                p->name = strdup(n2);
            }
        } else if (!strcmp(cmd, "ERASE")) {
            scanf("%s", n1);
            A *p = head;
            A *prev = NULL;
            while (p) {
                if (!strcmp(p->name, n1)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    free(p->name);
                    free(p->exp);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "DUMP")) {
            for (A *p = head; p; p = p->next) {
                printf("%s %s\n", p->name, p->exp);
            }
        }
    }
    return 0;
}