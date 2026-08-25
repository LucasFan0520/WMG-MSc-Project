// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Transaction {
    char *account;
    int amount;
    char *memo;
    struct Transaction *next;
} Transaction;

Transaction *head = NULL;

void add_transaction(char *account, int amount, char *memo) {
    Transaction *t = malloc(sizeof(Transaction));
    t->account = strdup(account);
    t->amount = amount;
    t->memo = strdup(memo);
    t->next = NULL;
    if (!head) {
        head = t;
    } else {
        Transaction *last = head;
        while (last->next) last = last->next;
        last->next = t;
    }
}

void reverse_transaction(char *account, int amount) {
    Transaction *prev = NULL;
    Transaction *curr = head;
    while (curr) {
        if (strcmp(curr->account, account) == 0 && curr->amount == amount) {
            Transaction *next = curr->next;
            if (prev) prev->next = next;
            else head = next;
            free(curr->account);
            free(curr->memo);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_balance(char *account) {
    int sum = 0;
    for (Transaction *t = head; t; t = t->next) {
        if (strcmp(t->account, account) == 0) sum += t->amount;
    }
    printf("%d\n", sum);
}

void report_transactions() {
    for (Transaction *t = head; t; t = t->next) {
        printf("%s %d %s\n", t->account, t->amount, t->memo);
    }
}

void parse_journal_line(char *line) {
    char account[100], amount_str[100];
    if (sscanf(line, "%99s%99s", account, amount_str) == 2) {
        char *p = strchr(line, ' ');
        while (p && *p == ' ') p++;
        p = strchr(p, ' ');
        while (p && *p == ' ') p++;
        if (p) {
            p[strcspn(p, "\n")] = 0;
            add_transaction(account, atoi(amount_str), p);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (file) {
            char line[1000];
            while (fgets(line, sizeof(line), file)) {
                parse_journal_line(line);
            }
            fclose(file);
        }
    }
    char cmd[20], account[100], memo[1000];
    int amount;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s%d", account, &amount);
            fgets(memo, sizeof(memo), stdin);
            while (memo[0] == ' ') memmove(memo, memo + 1, strlen(memo));
            memo[strcspn(memo, "\n")] = 0;
            add_transaction(account, amount, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            scanf("%99s%d", account, &amount);
            reverse_transaction(account, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            scanf("%99s", account);
            print_balance(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    return 0;
}