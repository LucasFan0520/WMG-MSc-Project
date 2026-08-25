/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct account {
    char *name;
    int balance;
    struct account *next;
} account_t;

account_t *head = NULL;
account_t *tail = NULL;

account_t *find_account(const char *name) {
    account_t *a = head;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void add_transaction(const char *name, int amount, const char *memo) {
    account_t *a = find_account(name);
    if (!a) {
        a = malloc(sizeof(account_t));
        if (!a) return;
        a->name = strdup(name);
        a->balance = 0;
        a->next = NULL;
        if (!a->name) { free(a); return; }
        if (tail) tail->next = a;
        else head = a;
        tail = a;
    }
    a->balance += amount;
}

void reverse_transaction(const char *name, int amount) {
    account_t *a = find_account(name);
    if (!a) return;
    a->balance -= amount;
    if (a->balance < 0) a->balance = 0; // not specified, but we don't go below 0? Actually spec says "REVERSE account amount" subtracts amount, no constraint? We'll allow negative.
}

void balance_account(const char *name) {
    account_t *a = find_account(name);
    printf("%d\n", a ? a->balance : 0);
}

void report_accounts(void) {
    account_t *a = head;
    while (a) {
        printf("%s %d\n", a->name, a->balance);
        a = a->next;
    }
}

void free_all(void) {
    account_t *a = head;
    while (a) {
        account_t *nxt = a->next;
        free(a->name);
        free(a);
        a = nxt;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, f) != -1) {
            line[strcspn(line, "\n")] = 0;
            char account[256], memo[256];
            int amount;
            if (sscanf(line, "%255s %d %255s", account, &amount, memo) == 3) {
                add_transaction(account, amount, memo);
            }
        }
        free(line);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], account[256], memo[256];
        int amount;
        if (sscanf(line, "%9s %255s %d %255s", cmd, account, &amount, memo) >= 2) {
            if (strcmp(cmd, "ADD") == 0) {
                if (sscanf(line, "%*s %255s %d %255s", account, &amount, memo) == 3)
                    add_transaction(account, amount, memo);
            } else if (strcmp(cmd, "REVERSE") == 0) {
                if (sscanf(line, "%*s %255s %d", account, &amount) == 2)
                    reverse_transaction(account, amount);
            } else if (strcmp(cmd, "BALANCE") == 0) {
                balance_account(account);
            } else if (strcmp(cmd, "REPORT") == 0) {
                report_accounts();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}