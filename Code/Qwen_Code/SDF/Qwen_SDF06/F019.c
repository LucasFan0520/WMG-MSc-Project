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

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char account[1024], memo[4096];
        int amount;
        if (sscanf(line, "%1023s %d %[^\n]", account, &amount, memo) == 3) {
            Transaction *t = malloc(sizeof(Transaction));
            if (!t) continue;
            t->account = strdup(account);
            t->amount = amount;
            t->memo = strdup(memo);
            t->next = head;
            head = t;
        }
    }
    fclose(f);
}

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    if (!t) return;
    t->account = strdup(account);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = head;
    head = t;
}

void reverse_transaction(const char *account, int amount) {
    Transaction *curr = head;
    Transaction *prev = NULL;
    while (curr) {
        if (strcmp(curr->account, account) == 0 && curr->amount == amount) {
            Transaction *next = curr->next;
            free(curr->account);
            free(curr->memo);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void balance_account(const char *account) {
    int bal = 0;
    for (Transaction *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->account, account) == 0) {
            bal += curr->amount;
        }
    }
    printf("%d\n", bal);
}

void report_transactions() {
    for (Transaction *curr = head; curr; curr = curr->next) {
        printf("%s %d %s\n", curr->account, curr->amount, curr->memo);
    }
}

void free_all() {
    Transaction *curr = head;
    while (curr) {
        Transaction *next = curr->next;
        free(curr->account);
        free(curr->memo);
        free(curr);
        curr = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char account[1024], memo[4096];
        int amount;
        
        int n = sscanf(line, "%15s %1023s %d %[^\n]", cmd, account, &amount, memo);
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            add_transaction(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0 && n >= 3) {
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0 && n >= 2) {
            balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    free_all();
    return 0;
}