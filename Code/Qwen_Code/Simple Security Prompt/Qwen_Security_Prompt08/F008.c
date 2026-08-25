// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct S {
    char *name;
    char *note;
    int score;
    struct S *next;
} S;

S *head = NULL;

int main() {
    char cmd[20];
    char name[100];
    char note[1000];
    int score;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD") || !strcmp(cmd, "UPDATE")) {
            scanf("%s %d", name, &score);
            fgets(note, 1000, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            S *p = head;
            while (p && strcmp(p->name, name)) {
                p = p->next;
            }
            if (p) {
                p->score = score;
                free(p->note);
                p->note = strdup(note);
            } else {
                S *ns = malloc(sizeof(S));
                ns->name = strdup(name);
                ns->score = score;
                ns->note = strdup(note);
                ns->next = head;
                head = ns;
            }
        } else if (!strcmp(cmd, "DROP")) {
            scanf("%s", name);
            S *p = head;
            S *prev = NULL;
            while (p) {
                if (!strcmp(p->name, name)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    free(p->name);
                    free(p->note);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "AVERAGE")) {
            int sum = 0;
            int cnt = 0;
            for (S *p = head; p; p = p->next) {
                sum += p->score;
                cnt++;
            }
            printf("%d\n", cnt ? sum / cnt : 0);
        } else if (!strcmp(cmd, "LIST")) {
            for (S *p = head; p; p = p->next) {
                printf("%s %d %s\n", p->name, p->score, p->note);
            }
        }
    }
    return 0;
}