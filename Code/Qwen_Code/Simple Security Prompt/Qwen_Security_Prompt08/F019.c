// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct T {
    char *acc;
    char *memo;
    int amt;
    struct T *next;
} T;

T *head = NULL;

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        char acc[100];
        int amt;
        if (sscanf(line, "%s %d", acc, &amt) == 2) {
            char *m = strchr(line, ' ');
            m = strchr(m + 1, ' ');
            if (m) {
                m++;
            } else {
                m = "";
            }
            T *nt = malloc(sizeof(T));
            nt->acc = strdup(acc);
            nt->amt = amt;
            nt->memo = strdup(m);
            nt->next = head;
            head = nt;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char acc[100];
    char memo[1000];
    int amt;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %d", acc, &amt);
            fgets(memo, 1000, stdin);
            memo[strcspn(memo, "\n")] = 0;
            if (memo[0] == ' ') {
                memmove(memo, memo + 1, strlen(memo));
            }
            T *nt = malloc(sizeof(T));
            nt->acc = strdup(acc);
            nt->amt = amt;
            nt->memo = strdup(memo);
            nt->next = head;
            head = nt;
        } else if (!strcmp(cmd, "REVERSE")) {
            scanf("%s %d", acc, &amt);
            T *p = head;
            while (p && (strcmp(p->acc, acc) || p->amt != amt)) {
                p = p->next;
            }
            if (p) {
                T *prev = NULL;
                T *curr = head;
                while (curr) {
                    if (curr == p) {
                        if (prev) {
                            prev->next = curr->next;
                        } else {
                            head = curr->next;
                        }
                        free(curr->acc);
                        free(curr->memo);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (!strcmp(cmd, "BALANCE")) {
            scanf("%s", acc);
            int bal = 0;
            for (T *p = head; p; p = p->next) {
                if (!strcmp(p->acc, acc)) {
                    bal += p->amt;
                }
            }
            printf("%d\n", bal);
        } else if (!strcmp(cmd, "REPORT")) {
            for (T *p = head; p; p = p->next) {
                printf("%s %d %s\n", p->acc, p->amt, p->memo);
            }
        }
    }
    return 0;
}