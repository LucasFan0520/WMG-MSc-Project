// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void add_transaction(const char *account, int amount, const char *memo) {
    Account *a = find_account(account);
    if (!a) {
        a = malloc(sizeof(Account));
        a->name = malloc(strlen(account) + 1);
        strcpy(a->name, account);
        a->balance = 0;
        a->next = accounts;
        accounts = a;
    }
    a->balance += amount;
}

void reverse_transaction(const char *account, int amount) {
    Account *a = find_account(account);
    if (!a) return;
    a->balance -= amount;
}

void balance_account(const char *account) {
    Account *a = find_account(account);
    printf("%d\n", a ? a->balance : 0);
}

void report_all(void) {
    Account *a = accounts;
    while (a) {
        printf("%s %d\n", a->name, a->balance);
        a = a->next;
    }
}

void load_file(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (!fp) return;
    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';
        char account[256], memo[4096];
        int amount;
        if (sscanf(line, "%255s %d", account, &amount) == 2) {
            const char *rest = line + strlen(account) + 1;
            while (*rest && *rest != ' ') rest++;
            while (*rest == ' ') rest++;
            if (*rest) strcpy(memo, rest);
            else memo[0] = '\0';
            add_transaction(account, amount, memo);
        }
    }
    fclose(fp);
}

void free_all(void) {
    Account *a = accounts;
    while (a) {
        Account *tmp = a->next;
        free(a->name);
        free(a);
        a = tmp;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char account[256];
        int amount;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s %d", account, &amount) == 2) {
                const char *memo = rest + strlen(account) + 1;
                while (*memo && *memo != ' ') memo++;
                while (*memo == ' ') memo++;
                if (*memo) add_transaction(account, amount, memo);
                else add_transaction(account, amount, "");
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(rest, "%255s %d", account, &amount) == 2) {
                reverse_transaction(account, amount);
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (sscanf(rest, "%255s", account) == 1) balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }
    free_all();
    return 0;
}