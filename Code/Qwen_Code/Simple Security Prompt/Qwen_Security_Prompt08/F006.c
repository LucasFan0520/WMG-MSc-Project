// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct I {
    char *name;
    int count;
    struct I *next;
} I;

I *head = NULL;
I *tail = NULL;

int main() {
    char cmd[20];
    char name[100];
    int c;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %d", name, &c);
            I *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                p->count += c;
            } else {
                I *ni = malloc(sizeof(I));
                ni->name = strdup(name);
                ni->count = c;
                ni->next = NULL;
                if (!head) {
                    head = tail = ni;
                } else {
                    tail->next = ni;
                    tail = ni;
                }
            }
        } else if (!strcmp(cmd, "TAKE")) {
            scanf("%s %d", name, &c);
            I *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                p->count -= c;
                if (p->count < 0) {
                    p->count = 0;
                }
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s", name);
            I *p = head;
            I *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    if (p == tail) {
                        tail = prev;
                    }
                    free(p->name);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "COUNT")) {
            scanf("%s", name);
            I *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            printf("%d\n", p ? p->count : 0);
        } else if (!strcmp(cmd, "REPORT")) {
            for (I *p = head; p; p = p->next) {
                printf("%s %d\n", p->name, p->count);
            }
        }
    }
    return 0;
}