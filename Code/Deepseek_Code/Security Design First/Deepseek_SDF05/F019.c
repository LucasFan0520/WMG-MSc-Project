// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Account {
    char *name;
    long balance;
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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_transaction(const char *account, long amount) {
    Account *a = find_account(account);
    if (!a) {
        a = malloc(sizeof(Account));
        if (!a) return;
        a->name = safe_strdup(account);
        a->balance = 0;
        a->next = accounts;
        accounts = a;
    }
    a->balance += amount;
}

void reverse_transaction(const char *account, long amount) {
    Account *a = find_account(account);
    if (a) {
        a->balance -= amount;
    }
}

void balance_account(const char *account) {
    Account *a = find_account(account);
    printf("%ld\n", a ? a->balance : 0);
}

void report_accounts(void) {
    Account *a = accounts;
    while (a) {
        printf("%s %ld\n", a->name, a->balance);
        a = a->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char account[512];
        char memo[MAX_LINE];
        long amount;
        if (sscanf(line, "%511s %ld %[^\n]", account, &amount, memo) == 3) {
            add_transaction(account, amount);
        }
    }
    fclose(f);
}

void free_all(void) {
    Account *a = accounts;
    while (a) {
        Account *next = a->next;
        free(a->name);
        free(a);
        a = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) load_file(argv[1]);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            long amount = 0;
            char memo[MAX_LINE] = "";
            sscanf(p, "%ld %[^\n]", &amount, memo);
            if (amount != 0) add_transaction(arg1, amount);
        } else if (strcmp(cmd, "REVERSE") == 0 && n >= 3) {
            long amount = atol(arg2);
            if (amount != 0) reverse_transaction(arg1, amount);
        } else if (strcmp(cmd, "BALANCE") == 0 && n >= 2) {
            balance_account(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_accounts();
        }
    }
    free_all();
    return 0;
}