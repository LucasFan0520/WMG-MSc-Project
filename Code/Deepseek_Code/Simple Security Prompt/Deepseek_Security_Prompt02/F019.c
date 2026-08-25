/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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

void free_account(Account *a) {
    free(a->name);
    free(a);
}

Account *get_or_create_account(const char *name) {
    Account *a = find_account(name);
    if (a) return a;
    a = malloc(sizeof(Account));
    a->name = strdup(name);
    a->balance = 0.0;
    a->next = accounts;
    accounts = a;
    return a;
}

void add_transaction(const char *account, double amount, const char *memo) {
    Account *a = get_or_create_account(account);
    a->balance += amount;
}

void reverse_transaction(const char *account, double amount) {
    Account *a = find_account(account);
    if (!a) return;
    a->balance -= amount;
    if (a->balance < 0) a->balance = 0;
}

void balance_account(const char *account) {
    Account *a = find_account(account);
    printf("%.2f\n", a ? a->balance : 0.0);
}

void report_accounts(void) {
    Account *a = accounts;
    while (a) {
        printf("%s %.2f\n", a->name, a->balance);
        a = a->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        char *nl = strchr(line, '\n');
        if (nl) *nl = '\0';
        char account[256];
        double amount;
        char memo[4096];
        if (sscanf(line, "%255s %lf", account, &amount) == 2) {
            char *p = line + strlen(account) + 1;
            while (*p == ' ') p++;
            while (*p && !isspace(*p)) p++;
            while (*p == ' ') p++;
            if (*p) add_transaction(account, amount, p);
            else add_transaction(account, amount, "");
        }
    }
    fclose(f);
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);

    char line[4096];
    char cmd[32];
    char account[256];
    double amount;
    char memo[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %lf", account, &amount) != 2) continue;
            p += strlen(account);
            while (*p == ' ') p++;
            p += (amount >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            add_transaction(account, amount, p);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %lf", account, &amount) == 2) reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", account) == 1) balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_accounts();
        }
    }

    while (accounts) {
        Account *tmp = accounts;
        accounts = accounts->next;
        free_account(tmp);
    }
    return 0;
}