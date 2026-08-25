// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Tx {
    char *account;
    int amount;
    char *memo;
    struct Tx *next;
} Tx;

Tx *txs = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char acc[256], memo[4096] = {0};
        int amt;
        if (sscanf(line, "%255s %d %4095[^\n]", acc, &amt, memo) >= 2) {
            Tx *t = malloc(sizeof(Tx));
            t->account = strdup(acc);
            t->amount = amt;
            t->memo = strdup(memo);
            t->next = txs;
            txs = t;
        }
    }
    fclose(f);
}

void add_tx(const char *acc, int amt, const char *memo) {
    Tx *t = malloc(sizeof(Tx));
    t->account = strdup(acc);
    t->amount = amt;
    t->memo = strdup(memo);
    t->next = txs;
    txs = t;
}

void reverse_tx(const char *acc, int amt) {
    Tx *curr = txs, *prev = NULL;
    while (curr) {
        if (strcmp(curr->account, acc) == 0 && curr->amount == amt) {
            if (prev) prev->next = curr->next;
            else txs = curr->next;
            free(curr->account);
            free(curr->memo);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void balance_acc(const char *acc) {
    int bal = 0;
    Tx *curr = txs;
    while (curr) {
        if (strcmp(curr->account, acc) == 0) bal += curr->amount;
        curr = curr->next;
    }
    printf("%d\n", bal);
}

void report_txs() {
    Tx *curr = txs;
    while (curr) {
        printf("%s %d %s\n", curr->account, curr->amount, curr->memo);
        curr = curr->next;
    }
}

void free_all() {
    Tx *curr = txs;
    while (curr) {
        Tx *next = curr->next;
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
        
        char cmd[32], acc[256], memo[4096] = {0};
        int amt;
        int n = sscanf(line, "%31s %255s %d %4095[^\n]", cmd, acc, &amt, memo);
        
        if (strcmp(cmd, "ADD") == 0 && n >= 3) add_tx(acc, amt, memo);
        else if (strcmp(cmd, "REVERSE") == 0 && n >= 3) reverse_tx(acc, amt);
        else if (strcmp(cmd, "BALANCE") == 0 && n >= 2) balance_acc(acc);
        else if (strcmp(cmd, "REPORT") == 0) report_txs();
    }
    free_all();
    return 0;
}