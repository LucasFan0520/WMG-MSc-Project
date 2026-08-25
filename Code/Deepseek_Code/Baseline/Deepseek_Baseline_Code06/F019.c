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

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    t->account = strdup(account);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = head;
    head = t;
}

void reverse_transaction(const char *account, int amount) {
    // add a negative transaction
    add_transaction(account, -amount, "reverse");
}

void balance_account(const char *account) {
    int balance = 0;
    Transaction *t = head;
    while (t) {
        if (strcmp(t->account, account) == 0) balance += t->amount;
        t = t->next;
    }
    printf("%d\n", balance);
}

void report_transactions() {
    Transaction *t = head;
    while (t) {
        printf("%s %d %s\n", t->account, t->amount, t->memo);
        t = t->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char account[256]; int amount; char memo[4096];
        if (sscanf(line, "%255s %d %[^\n]", account, &amount, memo) == 3) {
            add_transaction(account, amount, memo);
        }
    }
    fclose(f);
}

void free_all() {
    while (head) {
        Transaction *tmp = head;
        head = head->next;
        free(tmp->account);
        free(tmp->memo);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char account[256]; int amount; char memo[4096];
        int n = sscanf(line, "%31s %255s %d %[^\n]", cmd, account, &amount, memo);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_transaction(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0 && n == 3) {
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0 && n == 2) {
            balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_transactions();
        }
    }
    free_all();
    return 0;
}