// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct M {
    char *tag;
    char *msg;
    struct M *next;
} M;

M *head = NULL;

void add_msg(char *tag, char *msg) {
    M *n = malloc(sizeof(M));
    n->tag = strdup(tag);
    n->msg = strdup(msg);
    n->next = head;
    head = n;
}

M* find_tag(char *tag) {
    for (M *p = head; p; p = p->next) {
        if (!strcmp(p->tag, tag)) {
            return p;
        }
    }
    return NULL;
}

int main() {
    char cmd[20];
    char t1[100];
    char t2[100];
    char t3[100];
    char msg[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", t1);
            fgets(msg, 1000, stdin);
            msg[strcspn(msg, "\n")] = 0;
            if (msg[0] == ' ') {
                memmove(msg, msg + 1, strlen(msg));
            }
            add_msg(t1, msg);
        } else if (!strcmp(cmd, "CONCAT")) {
            scanf("%s %s %s", t1, t2, t3);
            M *m1 = find_tag(t1);
            M *m2 = find_tag(t2);
            if (m1 && m2) {
                char *nm = malloc(strlen(m1->msg) + strlen(m2->msg) + 2);
                sprintf(nm, "%s %s", m1->msg, m2->msg);
                add_msg(t3, nm);
                free(nm);
            } else if (m1) {
                add_msg(t3, m1->msg);
            } else if (m2) {
                add_msg(t3, m2->msg);
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s", t1);
            M *p = head;
            M *prev = NULL;
            while (p) {
                if (!strcmp(p->tag, t1)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    free(p->tag);
                    free(p->msg);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", t1);
            M *p = find_tag(t1);
            if (p) {
                for (char *c = p->msg; *c; c++) {
                    if (*c == ' ') {
                        printf("_");
                    } else {
                        printf("%c", *c);
                    }
                }
                printf("\n");
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (M *p = head; p; p = p->next) {
                printf("%s %s\n", p->tag, p->msg);
            }
        }
    }
    return 0;
}