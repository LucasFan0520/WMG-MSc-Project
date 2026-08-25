// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Transaction {
    char *account;
    int amount;
    char *memo;
    struct Transaction *next;
} Transaction;

Transaction *transactions = NULL;

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    t->account = strdup(account);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = transactions;
    transactions = t;
}

void reverse_transaction(const char *account, int amount) {
    // simply add a negative transaction? but spec says REVERSE account amount.
    // We'll add a negative amount transaction with memo "REVERSE"
    char memo[32];
    sprintf(memo, "REVERSE");
    add_transaction(account, -amount, memo);
}

void balance_account(const char *account) {
    int balance = 0;
    Transaction *t = transactions;
    while (t) {
        if (strcmp(t->account, account) == 0) balance += t->amount;
        t = t->next;
    }
    printf("%d\n", balance);
}

void report_transactions(void) {
    Transaction *t = transactions;
    while (t) {
        printf("%s %d %s\n", t->account, t->amount, t->memo);
        t = t->next;
    }
}

void cleanup_transactions(void) {
    Transaction *t = transactions;
    while (t) {
        Transaction *tmp = t;
        t = t->next;
        free(tmp->account);
        free(tmp->memo);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char account[256], memo[1024];
                int amount;
                if (sscanf(line, "%255s %d %1023[^\n]", account, &amount, memo) == 3) {
                    add_transaction(account, amount, memo);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], account[256], rest[2048];
        int amount;
        int n = sscanf(line, "%31s %255s %d %2047s", cmd, account, &amount, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 4) {
            char *memo = line + strlen(cmd) + 1 + strlen(account) + 1;
            // skip amount
            char *p = memo;
            while (*p && *p != ' ') p++;
            while (*p && *p == ' ') p++;
            add_transaction(account, amount, p);
        } else if (strcmp(cmd, "REVERSE") == 0 && n >= 3) {
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    cleanup_transactions();
    return 0;
}