// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Transaction {
    char *account;
    long amount; // can be negative for reversal? Actually REVERSE adds negative?
    char *memo;
    struct Transaction *next;
} Transaction;

Transaction *transactions = NULL;

long parse_amount(const char *s) {
    char *end;
    long v = strtol(s, &end, 10);
    if (*end != '\0') return 0; // invalid
    return v;
}

void add_transaction(const char *account, long amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    t->account = strdup(account);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = transactions;
    transactions = t;
}

void reverse_transaction(const char *account, long amount) {
    add_transaction(account, -amount, "REVERSAL");
}

void balance_account(const char *account) {
    Transaction *t = transactions;
    long bal = 0;
    while (t) {
        if (strcmp(t->account, account) == 0) {
            bal += t->amount;
        }
        t = t->next;
    }
    printf("%ld\n", bal);
}

void report_transactions() {
    Transaction *t = transactions;
    while (t) {
        printf("%s %ld:", t->account, t->amount);
        for (char *s = t->memo; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        t = t->next;
    }
}

void free_all() {
    Transaction *t = transactions;
    while (t) {
        Transaction *next = t->next;
        free(t->account);
        free(t->memo);
        free(t);
        t = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char account[256], memo[4096];
                char amount_str[256];
                if (sscanf(line, "%255s %255s %[^\n]", account, amount_str, memo) == 3) {
                    long amt = parse_amount(amount_str);
                    if (amt != 0) {
                        add_transaction(account, amt, memo);
                    }
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char account[256], amount_str[256], memo[4096];
            if (sscanf(rest, "%255s %255s %[^\n]", account, amount_str, memo) == 3) {
                long amt = parse_amount(amount_str);
                if (amt != 0) add_transaction(account, amt, memo);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char account[256], amount_str[256];
            if (sscanf(rest, "%255s %255s", account, amount_str) == 2) {
                long amt = parse_amount(amount_str);
                if (amt != 0) reverse_transaction(account, amt);
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char account[256];
            if (sscanf(rest, "%255s", account) == 1) {
                balance_account(account);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    free_all();
    return 0;
}