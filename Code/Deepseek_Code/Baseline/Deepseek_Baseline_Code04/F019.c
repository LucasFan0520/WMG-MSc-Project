/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Account {
    char *name;
    double balance;
    struct Account *next;
} Account;

Account *accounts = NULL;

Account *find_account(const char *name) {
    Account *a = accounts;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void add_transaction(const char *name, double amount) {
    Account *a = find_account(name);
    if (!a) {
        a = malloc(sizeof(Account));
        a->name = strdup(name);
        a->balance = 0.0;
        a->next = accounts;
        accounts = a;
    }
    a->balance += amount;
}

void reverse_transaction(const char *name, double amount) {
    Account *a = find_account(name);
    if (a) {
        a->balance -= amount;
    }
}

void balance_account(const char *name) {
    Account *a = find_account(name);
    if (a) printf("%.0f\n", a->balance);
}

void report_accounts(void) {
    Account *a = accounts;
    while (a) {
        printf("%s %.0f\n", a->name, a->balance);
        a = a->next;
    }
}

void free_accounts(void) {
    while (accounts) {
        Account *tmp = accounts;
        accounts = accounts->next;
        free(tmp->name);
        free(tmp);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char account[256], memo[256];
                double amount;
                if (sscanf(line, "%s %lf %s", account, &amount, memo) == 3) {
                    add_transaction(account, amount);
                }
            }
            fclose(f);
        }
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char account[256];
        double amount;
        int n = sscanf(line, "%s %s %lf", cmd, account, &amount);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 3) continue;
            add_transaction(account, amount);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (n < 3) continue;
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (n < 2) continue;
            balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_accounts();
        }
    }
    free_accounts();
    return 0;
}