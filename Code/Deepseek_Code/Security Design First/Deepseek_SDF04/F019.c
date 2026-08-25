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

Transaction *head = NULL;

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    if (!t) return;
    t->account = strdup(account);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = NULL;
    if (!t->account || !t->memo) {
        free(t->account); free(t->memo); free(t);
        return;
    }
    if (!head) head = t;
    else {
        Transaction *last = head;
        while (last->next) last = last->next;
        last->next = t;
    }
}

void reverse_transaction(const char *account, int amount) {
    // add a negative transaction
    char memo[64];
    snprintf(memo, sizeof(memo), "REVERSE");
    add_transaction(account, -amount, memo);
}

void balance_account(const char *account) {
    int bal = 0;
    Transaction *t = head;
    while (t) {
        if (strcmp(t->account, account) == 0)
            bal += t->amount;
        t = t->next;
    }
    printf("%d\n", bal);
}

void report_transactions(void) {
    Transaction *t = head;
    while (t) {
        printf("%s %d %s\n", t->account, t->amount, t->memo);
        t = t->next;
    }
}

void free_all(void) {
    Transaction *t = head;
    while (t) {
        Transaction *next = t->next;
        free(t->account);
        free(t->memo);
        free(t);
        t = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                line[strcspn(line, "\n")] = '\0';
                char account[256], memo[4096];
                int amount;
                if (sscanf(line, "%255s %d %4095[^\n]", account, &amount, memo) == 3) {
                    add_transaction(account, amount, memo);
                }
            }
            free(line);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], account[256], memo[4096];
        int amount;
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d %4095[^\n]", account, &amount, memo) == 3)
                add_transaction(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %255s %d", account, &amount) == 2)
                reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (sscanf(line, "%*s %255s", account) == 1)
                balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    free(line);
    free_all();
    return 0;
}