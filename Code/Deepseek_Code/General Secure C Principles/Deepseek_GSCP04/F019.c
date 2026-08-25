/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Account {
    char *name;
    long balance;
    struct Account *next;
} Account;

static Account *accounts = NULL;

static Account *find_account(const char *name) {
    Account *a = accounts;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

static void add_account(const char *name, long amount) {
    Account *a = find_account(name);
    if (a) {
        a->balance += amount;
    } else {
        Account *newa = malloc(sizeof(Account));
        if (!newa) { perror("malloc"); exit(1); }
        newa->name = strdup(name);
        newa->balance = amount;
        newa->next = accounts;
        accounts = newa;
    }
}

static void reverse_amount(const char *name, long amount) {
    Account *a = find_account(name);
    if (a) {
        a->balance -= amount;
    }
}

static void balance_account(const char *name) {
    Account *a = find_account(name);
    printf("%ld\n", a ? a->balance : 0);
}

static void report_accounts(void) {
    Account *a = accounts;
    while (a) {
        printf("%s %ld\n", a->name, a->balance);
        a = a->next;
    }
}

static int is_valid_number(const char *s) {
    while (*s) {
        if (!isdigit((unsigned char)*s) && *s != '-') return 0;
        s++;
    }
    return 1;
}

static void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *account = strtok(line, " ");
        char *amountstr = strtok(NULL, " ");
        char *memo = strtok(NULL, ""); // memo may contain underscores, but we ignore for balance
        if (!account || !amountstr) continue;
        if (!is_valid_number(amountstr)) continue;
        long amount = atol(amountstr);
        add_account(account, amount);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *account = strtok(NULL, " ");
            char *amountstr = strtok(NULL, " ");
            if (!account || !amountstr) continue;
            if (!is_valid_number(amountstr)) continue;
            long amount = atol(amountstr);
            add_account(account, amount);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *account = strtok(NULL, " ");
            char *amountstr = strtok(NULL, " ");
            if (!account || !amountstr) continue;
            if (!is_valid_number(amountstr)) continue;
            long amount = atol(amountstr);
            reverse_amount(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *account = strtok(NULL, " ");
            if (account) balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_accounts();
        }
    }
    // cleanup
    return 0;
}