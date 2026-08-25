/*
 * F019.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct acct {
    char *account;
    double balance;
    struct acct *next;
} Acct;

Acct *accounts = NULL;

Acct *find_acct(const char *account) {
    Acct *a = accounts;
    while (a) {
        if (strcmp(a->account, account) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void add_transaction(const char *account, double amount, const char *memo) {
    Acct *a = find_acct(account);
    if (!a) {
        a = malloc(sizeof(Acct));
        a->account = strdup(account);
        a->balance = 0.0;
        a->next = accounts;
        accounts = a;
    }
    a->balance += amount;
}

void reverse_transaction(const char *account, double amount) {
    Acct *a = find_acct(account);
    if (!a) return;
    a->balance -= amount;
}

void balance_account(const char *account) {
    Acct *a = find_acct(account);
    if (!a) {
        printf("0.00\n");
        return;
    }
    printf("%.2f\n", a->balance);
}

void report_all(void) {
    Acct *a = accounts;
    while (a) {
        printf("%s %.2f\n", a->account, a->balance);
        a = a->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char account[256], memo[1024];
                double amount;
                if (sscanf(line, "%255s %lf", account, &amount) == 2) {
                    char *rest = line + strlen(account) + 1;
                    while (*rest == ' ') rest++;
                    while (*rest && *rest != ' ') rest++;
                    while (*rest == ' ') rest++;
                    strcpy(memo, rest);
                    add_transaction(account, amount, memo);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], account[256], memo[1024];
        double amount;
        if (sscanf(line, "%31s %255s %lf", cmd, account, &amount) == 3) {
            char *rest = line + strlen(cmd) + strlen(account) + 1;
            while (*rest == ' ') rest++;
            while (*rest && *rest != ' ') rest++;
            while (*rest == ' ') rest++;
            strcpy(memo, rest);
            if (strcmp(cmd, "ADD") == 0) {
                add_transaction(account, amount, memo);
            } else if (strcmp(cmd, "REVERSE") == 0) {
                reverse_transaction(account, amount);
            } else if (strcmp(cmd, "BALANCE") == 0) {
                balance_account(account);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_all();
            }
        }
    }
    return 0;
}