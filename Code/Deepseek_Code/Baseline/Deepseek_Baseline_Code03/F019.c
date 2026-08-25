// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct transaction {
    char *account;
    int amount;
    char *memo;
    struct transaction *next;
};

struct transaction *transactions = NULL;

void add_transaction(char *account, int amount, char *memo) {
    struct transaction *t = malloc(sizeof(struct transaction));
    t->account = malloc(strlen(account) + 1);
    strcpy(t->account, account);
    t->amount = amount;
    t->memo = malloc(strlen(memo) + 1);
    strcpy(t->memo, memo);
    t->next = transactions;
    transactions = t;
}

void reverse_transaction(char *account, int amount) {
    add_transaction(account, -amount, "REVERSE");
}

void balance_account(char *account) {
    int bal = 0;
    struct transaction *t = transactions;
    while (t) {
        if (strcmp(t->account, account) == 0) bal += t->amount;
        t = t->next;
    }
    printf("%d\n", bal);
}

void report_transactions() {
    struct transaction *t = transactions;
    while (t) {
        printf("%s %d %s\n", t->account, t->amount, t->memo);
        t = t->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[4096];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = 0;
            char account[256], memo[4096];
            int amount;
            if (sscanf(line, "%255s %d %4095[^\n]", account, &amount, memo) == 3) {
                add_transaction(account, amount, memo);
            }
        }
        fclose(f);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char account[256], memo[4096];
        int amount;
        int n = sscanf(line, "%31s %255s %d %4095[^\n]", cmd, account, &amount, memo);
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            add_transaction(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0 && n >= 3) {
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0 && n >= 2) {
            balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    return 0;
}