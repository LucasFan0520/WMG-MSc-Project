// F019.c
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

void add_transaction(const char *account, int amount, const char *memo) {
    Account *a = find_account(account);
    if (!a) {
        a = malloc(sizeof(Account));
        if (!a) return;
        a->name = strdup(account);
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

void report_accounts(void) {
    Account *a = accounts;
    while (a) {
        printf("%s %d\n", a->name, a->balance);
        a = a->next;
    }
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

int is_integer(const char *s) {
    if (*s == '-' || *s == '+') s++;
    if (!*s) return 0;
    while (*s) {
        if (!isdigit(*s)) return 0;
        s++;
    }
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Missing file argument\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        // format: account amount memo (memo may contain underscores)
        char account[256], memo[4096];
        int amount;
        char *p = line;
        char *tok = strtok(p, " ");
        if (!tok) continue;
        strcpy(account, tok);
        tok = strtok(NULL, " ");
        if (!tok) continue;
        if (!is_integer(tok)) continue;
        amount = atoi(tok);
        tok = strtok(NULL, "");
        if (tok) strcpy(memo, tok);
        else memo[0] = '\0';
        add_transaction(account, amount, memo);
    }
    fclose(f);

    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char account[256];
            int amount;
            if (sscanf(p, "%255s %d", account, &amount) != 2) continue;
            char *memo_start = strchr(p, ' ');
            if (!memo_start) continue;
            memo_start = strchr(memo_start + 1, ' ');
            if (!memo_start) continue;
            while (*memo_start == ' ') memo_start++;
            add_transaction(account, amount, memo_start);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char account[256];
            int amount;
            if (sscanf(p, "%255s %d", account, &amount) != 2) continue;
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char account[256];
            if (sscanf(p, "%255s", account) != 1) continue;
            balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_accounts();
        }
    }
    free_all();
    return 0;
}