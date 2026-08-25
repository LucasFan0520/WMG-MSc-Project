// filename: F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Transaction {
    char *account;
    double amount;
    char *memo;
    struct Transaction *next;
} Transaction;

static Transaction *txns = NULL;

static void cleanup(void) {
    while (txns) {
        Transaction *tmp = txns;
        txns = txns->next;
        free(tmp->account);
        free(tmp->memo);
        free(tmp);
    }
}

static void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char *acc = strtok(line, " ");
        char *amtstr = strtok(NULL, " ");
        char *memo = strtok(NULL, "");
        if (!acc || !amtstr || !memo) continue;
        char *end;
        double amt = strtod(amtstr, &end);
        if (end == amtstr) continue; // invalid
        Transaction *t = malloc(sizeof(Transaction));
        if (!t) continue;
        t->account = strdup(acc);
        t->memo = strdup(memo);
        if (!t->account || !t->memo) {
            free(t->account);
            free(t->memo);
            free(t);
            continue;
        }
        t->amount = amt;
        t->next = txns;
        txns = t;
    }
    free(line);
    fclose(f);
}

static void add_transaction(const char *account, double amount, const char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    if (!t) return;
    t->account = strdup(account);
    t->memo = strdup(memo);
    if (!t->account || !t->memo) {
        free(t->account);
        free(t->memo);
        free(t);
        return;
    }
    t->amount = amount;
    t->next = txns;
    txns = t;
}

static void reverse_transaction(const char *account, double amount) {
    // we just add a negative transaction with empty memo? Or we just remove? Spec says REVERSE account amount - likely add a negative record.
    char *memo = strdup("");
    if (!memo) return;
    add_transaction(account, -amount, memo);
    free(memo);
}

static double balance(const char *account) {
    double bal = 0.0;
    for (Transaction *t = txns; t; t = t->next)
        if (strcmp(t->account, account) == 0)
            bal += t->amount;
    return bal;
}

static void report(void) {
    for (Transaction *t = txns; t; t = t->next)
        printf("%s %g %s\n", t->account, t->amount, t->memo);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        double amt = strtod(arg2, NULL);
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_transaction(arg1, amt, arg3);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (*arg1 && *arg2) reverse_transaction(arg1, amt);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (*arg1) printf("%g\n", balance(arg1));
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free(line);
    return 0;
}