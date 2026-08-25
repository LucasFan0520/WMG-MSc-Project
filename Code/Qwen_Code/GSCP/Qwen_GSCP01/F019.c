// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Transaction {
    char *account;
    int amount;
    char *memo;
    struct Transaction *next;
} Transaction;

Transaction *transactions = NULL;

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    if (!t) exit(EXIT_FAILURE);
    t->account = strdup(account);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = NULL;
    if (!transactions) {
        transactions = t;
    } else {
        Transaction *curr = transactions;
        while (curr->next) curr = curr->next;
        curr->next = t;
    }
}

void reverse_transaction(const char *account, int amount) {
    Transaction *curr = transactions;
    while (curr) {
        if (strcmp(curr->account, account) == 0 && curr->amount == amount) {
            curr->amount = -curr->amount;
            return;
        }
        curr = curr->next;
    }
}

void balance_account(const char *account) {
    int sum = 0;
    Transaction *curr = transactions;
    while (curr) {
        if (strcmp(curr->account, account) == 0) {
            sum += curr->amount;
        }
        curr = curr->next;
    }
    printf("%d\n", sum);
}

void report_transactions() {
    Transaction *curr = transactions;
    while (curr) {
        printf("%s %d %s\n", curr->account, curr->amount, curr->memo);
        curr = curr->next;
    }
}

void free_all() {
    Transaction *curr = transactions;
    while (curr) {
        Transaction *next = curr->next;
        free(curr->account);
        free(curr->memo);
        free(curr);
        curr = next;
    }
}

int is_valid_amount(const char *str) {
    if (!str || !*str) return 0;
    for (int i = 0; str[i]; i++) {
        if (i == 0 && str[i] == '-') continue;
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char account[256] = {0};
        char amount_str[64] = {0};
        char memo[700] = {0};
        
        int matched = sscanf(line, "%255s %63s %[^\n]", account, amount_str, memo);
        if (matched >= 2 && is_valid_amount(amount_str)) {
            int amount = atoi(amount_str);
            if (matched == 2) {
                add_transaction(account, amount, "");
            } else {
                for (int i = 0; memo[i]; i++) {
                    if (memo[i] == '_') memo[i] = ' ';
                }
                add_transaction(account, amount, memo);
            }
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char account[256] = {0};
        char amount_str[64] = {0};
        char memo[3500] = {0};
        
        int matched = sscanf(line, "%31s %255s %63s %[^\n]", cmd, account, amount_str, memo);
        
        if (strcmp(cmd, "ADD") == 0 && matched >= 3 && is_valid_amount(amount_str)) {
            int amount = atoi(amount_str);
            if (matched == 3) {
                add_transaction(account, amount, "");
            } else {
                add_transaction(account, amount, memo);
            }
        } else if (strcmp(cmd, "REVERSE") == 0 && matched >= 3 && is_valid_amount(amount_str)) {
            reverse_transaction(account, atoi(amount_str));
        } else if (strcmp(cmd, "BALANCE") == 0) {
            balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    free_all();
    return 0;
}