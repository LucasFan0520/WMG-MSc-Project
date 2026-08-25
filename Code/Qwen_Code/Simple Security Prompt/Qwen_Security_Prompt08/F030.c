// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rule {
    char *old;
    char *new;
    struct Rule *next;
} Rule;

Rule *head = NULL;
int count = 0;

Rule* get_at(int idx) {
    Rule *p = head;
    for (int i = 0; i < idx && p; i++) p = p->next;
    return p;
}

int main() {
    char cmd[20];
    char old[100];
    char new_str[100];
    char text[1000];
    int i, j;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s", old, new_str);
            Rule *r = malloc(sizeof(Rule));
            r->old = strdup(old);
            r->new = strdup(new_str);
            r->next = NULL;
            if (!head) head = r;
            else {
                Rule *p = head;
                while (p->next) p = p->next;
                p->next = r;
            }
            count++;
        } else if (!strcmp(cmd, "APPLY")) {
            scanf("%s", text);
            char *found = NULL;
            for (Rule *r = head; r; r = r->next) {
                if (strstr(text, r->old)) {
                    found = r->old;
                    break;
                }
            }
            if (found) {
                for (Rule *r = head; r; r = r->next) {
                    if (!strcmp(r->old, found)) {
                        char *pos = strstr(text, found);
                        int len = strlen(found);
                        char res[2000];
                        strncpy(res, text, pos - text);
                        res[pos - text] = 0;
                        strcat(res, r->new);
                        strcat(res, pos + len);
                        printf("%s\n", res);
                        break;
                    }
                }
            } else {
                printf("%s\n", text);
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", old);
            Rule *p = head;
            Rule *prev = NULL;
            while (p) {
                if (!strcmp(p->old, old)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    free(p->old);
                    free(p->new);
                    free(p);
                    count--;
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "SWAP")) {
            scanf("%d %d", &i, &j);
            if (i < 0 || i >= count || j < 0 || j >= count || i == j) continue;
            Rule *ri = get_at(i);
            Rule *rj = get_at(j);
            char *tmp_old = ri->old;
            char *tmp_new = ri->new;
            ri->old = rj->old;
            ri->new = rj->new;
            rj->old = tmp_old;
            rj->new = tmp_new;
        } else if (!strcmp(cmd, "RULES")) {
            for (Rule *r = head; r; r = r->next) {
                printf("%s %s\n", r->old, r->new);
            }
        }
    }
    return 0;
}