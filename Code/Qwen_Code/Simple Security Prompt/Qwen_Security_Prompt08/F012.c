// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Q {
    char *name;
    char *note;
    struct Q *next;
} Q;

Q *head = NULL;
Q *tail = NULL;

int main() {
    char cmd[20];
    char name[100];
    char note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "JOIN") || !strcmp(cmd, "REJOIN")) {
            scanf("%s", name);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            Q *nq = malloc(sizeof(Q));
            nq->name = strdup(name);
            nq->note = strdup(note);
            nq->next = NULL;
            if (!head) {
                head = tail = nq;
            } else {
                tail->next = nq;
                tail = nq;
            }
        } else if (!strcmp(cmd, "SERVE")) {
            if (head) {
                printf("%s\n", head->name);
                Q *tmp = head;
                head = head->next;
                if (!head) {
                    tail = NULL;
                }
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
        } else if (!strcmp(cmd, "CANCEL")) {
            scanf("%s", name);
            Q *p = head;
            Q *prev = NULL;
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
                    free(p->note);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name);
            Q *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                printf("%s\n", p->note);
            }
        } else if (!strcmp(cmd, "QUEUE")) {
            for (Q *p = head; p; p = p->next) {
                printf("%s\n", p->name);
            }
        }
    }
    return 0;
}