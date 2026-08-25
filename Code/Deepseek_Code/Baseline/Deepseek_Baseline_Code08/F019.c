// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Transaction {
    char *account;
    int amount; // can be negative for REVERSE
    char *memo;
    struct Transaction *next;
} Transaction;

Transaction *head = NULL;

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *new = malloc(sizeof(Transaction));
    new->account = strdup(account);
    new->amount = amount;
    new->memo = strdup(memo);
    new->next = NULL;
    if (!head) head = new;
    else {
        Transaction *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void reverse_transaction(const char *account, int amount) {
    // amount is absolute? spec says REVERSE account amount, so we store negative amount
    add_transaction(account, -amount, "REVERSE");
}

int balance(const char *account) {
    int bal = 0;
    Transaction *cur = head;
    while (cur) {
        if (strcmp(cur->account, account) == 0) {
            bal += cur->amount;
        }
        cur = cur->next;
    }
    return bal;
}

void report_all() {
    Transaction *cur = head;
    while (cur) {
        printf("%s %d %s\n", cur->account, cur->amount, cur->memo);
        cur = cur->next;
    }
}

void free_all() {
    Transaction *cur = head;
    while (cur) {
        Transaction *tmp = cur;
        cur = cur->next;
        free(tmp->account);
        free(tmp->memo);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
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
    char cmdline[4096];
    while (fgets(cmdline, sizeof(cmdline), stdin)) {
        cmdline[strcspn(cmdline, "\n")] = 0;
        if (strcmp(cmdline, "END") == 0) break;
        char *cmd = strtok(cmdline, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *account = strtok(NULL, " ");
            if (!account) continue;
            char *amount_str = strtok(NULL, " ");
            if (!amount_str) continue;
            int amount = atoi(amount_str);
            char *memo = strtok(NULL, "");
            if (!memo) memo = "";
            add_transaction(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *account = strtok(NULL, " ");
            if (!account) continue;
            char *amount_str = strtok(NULL, " ");
            if (!amount_str) continue;
            int amount = atoi(amount_str);
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *account = strtok(NULL, " ");
            if (!account) continue;
            printf("%d\n", balance(account));
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}