// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Trans {
    char *account, *memo;
    int amount;
    struct Trans *next;
} Trans;

Trans *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        char acc[256], memo[1024];
        int amt;
        if (sscanf(line, "%s %d %[^\n]", acc, &amt, memo) == 3) {
            Trans *t = malloc(sizeof(Trans));
            t->account = strdup(acc); t->amount = amt; t->memo = strdup(memo); t->next = head; head = t;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char cmd[20], acc[256], memo[1024];
    int amt;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            if (scanf("%s %d", acc, &amt) == 2) {
                fgets(memo, sizeof(memo), stdin);
                if (memo[0] == ' ') memmove(memo, memo + 1, strlen(memo));
                size_t l = strlen(memo);
                if (l > 0 && memo[l-1] == '\n') memo[l-1] = '\0';
                Trans *t = malloc(sizeof(Trans));
                t->account = strdup(acc); t->amount = amt; t->memo = strdup(memo); t->next = head; head = t;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            scanf("%s %d", acc, &amt);
            Trans *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->account, acc) == 0 && curr->amount == amt) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->account); free(curr->memo); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            scanf("%s", acc);
            int bal = 0;
            for (Trans *t = head; t; t = t->next)
                if (strcmp(t->account, acc) == 0) bal += t->amount;
            printf("%d\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Trans *t = head; t; t = t->next) printf("%s %d %s\n", t->account, t->amount, t->memo);
        }
    }
    return 0;
}