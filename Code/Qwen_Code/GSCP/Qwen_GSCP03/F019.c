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

void free_list() {
    while (head) {
        Trans *t = head; head = head->next;
        free(t->account); free(t->memo); free(t);
    }
}

void add_trans(const char *acc, int amt, const char *memo) {
    Trans *t = malloc(sizeof(Trans));
    t->account = strdup(acc); t->amount = amt; t->memo = strdup(memo);
    t->next = head; head = t;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char acc[256], memo[256] = "";
                int amt;
                if (sscanf(line, "%255s %d %255[^\n]", acc, &amt, memo) >= 2) {
                    add_trans(acc, amt, memo);
                }
            }
            fclose(f);
        }
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], acc[256], memo[256] = "";
        int amt;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d %255[^\n]", acc, &amt, memo) < 2) continue;
            add_trans(acc, amt, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %255s %d", acc, &amt) != 2) continue;
            Trans *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->account, acc) == 0 && curr->amount == amt) {
                    Trans *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->account); free(tmp->memo); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (sscanf(line, "%*s %255s", acc) != 1) continue;
            int bal = 0;
            for (Trans *t = head; t; t = t->next)
                if (strcmp(t->account, acc) == 0) bal += t->amount;
            printf("%d\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Trans *t = head; t; t = t->next)
                printf("%s %d %s\n", t->account, t->amount, t->memo);
        }
    }
    free_list();
    return 0;
}