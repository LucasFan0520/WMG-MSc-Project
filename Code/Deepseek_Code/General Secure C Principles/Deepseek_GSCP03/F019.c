/* F019.c */
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

Transaction *find_transaction(const char *account) {
    Transaction *t = head;
    while (t) {
        if (strcmp(t->account, account) == 0) return t;
        t = t->next;
    }
    return NULL;
}

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *t = find_transaction(account);
    if (t) {
        t->amount += amount;
        return;
    }
    Transaction *new_t = malloc(sizeof(Transaction));
    if (!new_t) return;
    new_t->account = strdup(account);
    new_t->amount = amount;
    new_t->memo = strdup(memo);
    new_t->next = head;
    head = new_t;
}

void reverse_transaction(const char *account, int amount) {
    Transaction *t = find_transaction(account);
    if (!t) return;
    t->amount -= amount;
}

void balance_account(const char *account) {
    Transaction *t = find_transaction(account);
    printf("%d\n", t ? t->amount : 0);
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
        Transaction *tmp = t;
        t = t->next;
        free(tmp->account);
        free(tmp->memo);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char account[256], memo[256];
                int amount;
                if (sscanf(line, "%255s %d %255s", account, &amount, memo) == 3) {
                    add_transaction(account, amount, memo);
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char account[256], memo[256];
            int amount;
            if (sscanf(line + 4, "%255s %d %255s", account, &amount, memo) == 3) {
                add_transaction(account, amount, memo);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char account[256];
            int amount;
            if (sscanf(line + 8, "%255s %d", account, &amount) == 2) {
                reverse_transaction(account, amount);
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            balance_account(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    free_all();
    return 0;
}