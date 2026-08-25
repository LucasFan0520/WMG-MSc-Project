/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Account {
    char *name;
    int balance;
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

void add_transaction(const char *name, int amount, const char *memo) {
    Account *a = find_account(name);
    if (!a) {
        a = malloc(sizeof(Account));
        if (!a) return;
        a->name = strdup(name);
        a->balance = 0;
        a->next = accounts;
        accounts = a;
    }
    a->balance += amount;
}

void reverse_transaction(const char *name, int amount) {
    Account *a = find_account(name);
    if (!a) return;
    a->balance -= amount;
}

void balance_account(const char *name) {
    Account *a = find_account(name);
    printf("%d\n", a ? a->balance : 0);
}

void report_all(void) {
    Account *a = accounts;
    while (a) {
        printf("%s %d\n", a->name, a->balance);
        a = a->next;
    }
}

int parse_amount(const char *s) {
    int sign = 1;
    if (*s == '-') { sign = -1; s++; }
    for (const char *p = s; *p; p++) if (!isdigit(*p)) return 0;
    int val = atoi(s);
    return sign * val;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char account[256], memo[2048];
                int amount;
                int n = sscanf(line, "%255s %d %2047s", account, &amount, memo);
                if (n >= 2) {
                    add_transaction(account, amount, (n == 3) ? memo : "");
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            int amt = parse_amount(arg2);
            if (amt == 0 && strcmp(arg2, "0") != 0) continue;
            char *memo = (n == 4) ? arg3 : "";
            add_transaction(arg1, amt, memo);
        } else if (strcmp(cmd, "REVERSE") == 0 && n >= 3) {
            int amt = parse_amount(arg2);
            if (amt == 0 && strcmp(arg2, "0") != 0) continue;
            reverse_transaction(arg1, amt);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            balance_account(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    while (accounts) {
        Account *tmp = accounts;
        accounts = accounts->next;
        free(tmp->name);
        free(tmp);
    }
    return 0;
}