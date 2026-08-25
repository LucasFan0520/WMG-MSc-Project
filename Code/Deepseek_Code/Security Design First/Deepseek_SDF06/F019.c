// F019.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct {
    char *account;
    int balance;
} Account;

static Account *accounts = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_account(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(accounts[i].account, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_account(const char *name, int amount) {
    ssize_t idx = find_account(name);
    if (idx != -1) {
        accounts[idx].balance += amount;
        return;
    }
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Account *new = realloc(accounts, capacity * sizeof(Account));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        accounts = new;
    }
    accounts[count].account = strdup(name);
    if (!accounts[count].account) { fprintf(stderr, "Memory error\n"); exit(1); }
    accounts[count].balance = amount;
    count++;
}

static void reverse(const char *name, int amount) {
    ssize_t idx = find_account(name);
    if (idx == -1) return;
    accounts[idx].balance -= amount;
}

static void balance(const char *name) {
    ssize_t idx = find_account(name);
    printf("%d\n", idx == -1 ? 0 : accounts[idx].balance);
}

static void report_accounts(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %d\n", accounts[i].account, accounts[i].balance);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++)
        free(accounts[i].account);
    free(accounts);
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, f)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        char acct[256], memo[256];
        int amt;
        if (sscanf(line, "%255s %d %255s", acct, &amt, memo) == 3) {
            add_account(acct, amt);
        }
    }
    free(line);
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);

    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], acct[256], memo[256];
    int amount;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d %255s", acct, &amount, memo) == 3 && amount > 0)
                add_account(acct, amount);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %255s %d", acct, &amount) == 2 && amount > 0)
                reverse(acct, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (sscanf(line, "%*s %255s", acct) == 1)
                balance(acct);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_accounts();
        }
    }

    free(line);
    free_all();
    return 0;
}