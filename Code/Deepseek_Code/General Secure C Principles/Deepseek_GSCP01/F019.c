/* F019.c */
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

Transaction *txs = NULL;

int is_number(const char *s) {
    for (const char *p = s; *p; p++) if (!isdigit(*p) && *p != '-') return 0;
    return 1;
}

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    t->account = strdup(account);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = txs;
    txs = t;
}

void reverse_transaction(const char *account, int amount) {
    add_transaction(account, -amount, "REVERSAL");
}

void balance_account(const char *account) {
    int bal = 0;
    Transaction *t = txs;
    while (t) {
        if (strcmp(t->account, account) == 0) bal += t->amount;
        t = t->next;
    }
    printf("%d\n", bal);
}

void report_all(void) {
    Transaction *t = txs;
    while (t) {
        printf("%s %d ", t->account, t->amount);
        for (char *c = t->memo; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        t = t->next;
    }
}

void free_all(void) {
    Transaction *t = txs;
    while (t) {
        Transaction *next = t->next;
        free(t->account);
        free(t->memo);
        free(t);
        t = next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[1024];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = '\0';
            char account[256], memo[256];
            int amount;
            if (sscanf(line, "%255s %d %255[^\n]", account, &amount, memo) == 3) {
                if (is_number(memo)) { } 
                add_transaction(account, amount, memo);
            }
        }
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char account[256], memo[256];
            int amount;
            if (sscanf(line + 4, "%255s %d %255[^\n]", account, &amount, memo) == 3) {
                if (is_number(memo)) { }
                add_transaction(account, amount, memo);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char account[256];
            int amount;
            if (sscanf(line + 8, "%255s %d", account, &amount) == 2)
                reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            balance_account(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    free(line);
    return 0;
}