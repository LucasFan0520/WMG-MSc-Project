// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Transaction {
    char *account;
    double amount;
    char *memo;
    struct Transaction *next;
} Transaction;

Transaction *head = NULL;

void add_transaction(const char *account, double amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    t->account = malloc(strlen(account) + 1);
    strcpy(t->account, account);
    t->amount = amount;
    t->memo = malloc(strlen(memo) + 1);
    strcpy(t->memo, memo);
    t->next = head;
    head = t;
}

void reverse_transaction(const char *account, double amount) {
    // reverse means subtract? We'll add a negative amount with memo "REVERSE"
    char memo[32];
    sprintf(memo, "REVERSE");
    add_transaction(account, -amount, memo);
}

void balance_account(const char *account) {
    double bal = 0;
    Transaction *t = head;
    while (t) {
        if (strcmp(t->account, account) == 0) {
            bal += t->amount;
        }
        t = t->next;
    }
    // print as integer? The spec says account amount memo, but balance should be printed? Not specified. Print with .2f
    printf("%.2f\n", bal);
}

void report_all(void) {
    Transaction *t = head;
    while (t) {
        printf("%s %.2f ", t->account, t->amount);
        for (char *c = t->memo; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        t = t->next;
    }
}

void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char account[256], memo[4096];
        double amount;
        if (sscanf(line, "%255s %lf", account, &amount) == 2) {
            // memo is the rest of line after amount
            char *p = line;
            while (*p && *p != ' ') p++;
            if (*p) p++;
            while (*p && *p != ' ') p++;
            if (*p) p++;
            while (*p == ' ') p++;
            if (strlen(p) > 0) {
                add_transaction(account, amount, p);
            } else {
                add_transaction(account, amount, "");
            }
        }
    }
    fclose(f);
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
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *account = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            double amount;
            if (sscanf(p, "%lf", &amount) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            add_transaction(account, amount, p);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *account = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            double amount;
            if (sscanf(p, "%lf", &amount) != 1) continue;
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
    return 0;
}