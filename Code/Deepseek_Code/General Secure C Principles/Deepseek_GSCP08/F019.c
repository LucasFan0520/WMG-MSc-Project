// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Trans { char *account; double amount; char *memo; struct Trans *next; } Trans;
Trans *transactions = NULL;

void add_trans(const char *account, double amount, const char *memo) {
    Trans *t = malloc(sizeof(Trans));
    if (!t) return;
    t->account = strdup(account); t->amount = amount; t->memo = strdup(memo);
    t->next = transactions; transactions = t;
}

void reverse_trans(const char *account, double amount) {
    // add a negative amount
    add_trans(account, -amount, "REVERSE");
}

double balance(const char *account) {
    double bal = 0;
    for (Trans *t = transactions; t; t = t->next)
        if (strcmp(t->account, account) == 0) bal += t->amount;
    return bal;
}

void report_trans(void) {
    for (Trans *t = transactions; t; t = t->next)
        printf("%s %.2f %s\n", t->account, t->amount, t->memo);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char account[256], memo[4096];
                double amount;
                if (sscanf(line, "%255s %lf", account, &amount) == 2) {
                    char *m = line + strlen(account) + 1;
                    while (*m == ' ') m++;
                    while (isdigit(*m) || *m == '.' || *m == '-' || *m == '+') m++;
                    while (*m == ' ') m++;
                    strcpy(memo, m);
                    add_trans(account, amount, memo);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char account[256], memo[4096];
            double amount;
            if (sscanf(rest, "%255s %lf", account, &amount) == 2) {
                char *m = rest + strlen(account) + 1;
                while (*m == ' ') m++;
                while (isdigit(*m) || *m == '.' || *m == '-' || *m == '+') m++;
                while (*m == ' ') m++;
                strcpy(memo, m);
                add_trans(account, amount, memo);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char account[256];
            double amount;
            if (sscanf(rest, "%255s %lf", account, &amount) == 2) reverse_trans(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char account[256];
            if (sscanf(rest, "%255s", account) == 1) printf("%.2f\n", balance(account));
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_trans();
        }
    }
    while (transactions) { Trans *t = transactions; transactions = t->next; free(t->account); free(t->memo); free(t); }
    return 0;
}