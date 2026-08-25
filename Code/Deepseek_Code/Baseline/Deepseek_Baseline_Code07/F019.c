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
Transaction *tail = NULL;

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    t->account = malloc(strlen(account)+1);
    strcpy(t->account, account);
    t->amount = amount;
    t->memo = malloc(strlen(memo)+1);
    strcpy(t->memo, memo);
    t->next = NULL;
    if (tail) tail->next = t;
    else head = t;
    tail = t;
}

void reverse_transaction(const char *account, int amount) {
    // add a negative transaction
    char neg_memo[32];
    snprintf(neg_memo, sizeof(neg_memo), "REVERSE %d", amount);
    add_transaction(account, -amount, neg_memo);
}

int balance_account(const char *account) {
    int bal = 0;
    Transaction *t = head;
    while (t) {
        if (strcmp(t->account, account) == 0) bal += t->amount;
        t = t->next;
    }
    return bal;
}

void report_all(void) {
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
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *account = strtok(line, " ");
                if (!account) continue;
                char *amount_str = strtok(NULL, " ");
                if (!amount_str) continue;
                int amount = atoi(amount_str);
                char *memo = strtok(NULL, "");
                if (!memo) memo = "";
                add_transaction(account, amount, memo);
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *account = strtok(rest, " ");
            if (!account) continue;
            char *amount_str = strtok(NULL, " ");
            if (!amount_str) continue;
            int amount = atoi(amount_str);
            char *memo = strtok(NULL, "");
            if (!memo) memo = "";
            add_transaction(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *account = strtok(rest, " ");
            if (!account) continue;
            char *amount_str = strtok(NULL, " ");
            if (!amount_str) continue;
            int amount = atoi(amount_str);
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *account = strtok(rest, " ");
            if (account) printf("%d\n", balance_account(account));
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}