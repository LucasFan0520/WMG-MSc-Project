/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    t->next = head;
    head = t;
}

int balance_account(char *account) {
    int bal = 0;
    for (Transaction *t = head; t; t = t->next)
        if (strcmp(t->account, account) == 0)
            bal += t->amount;
    return bal;
}

void reverse_transaction(char *account, int amount) {
    // add a negative amount
    add_transaction(account, -amount, "reverse");
}

void report_all(void) {
    // group by account
    // simple: print each transaction
    for (Transaction *t = head; t; t = t->next)
        printf("%s %d %s\n", t->account, t->amount, t->memo);
}

void free_all(void) {
    Transaction *t = head;
    while (t) {
        Transaction *next = t->next;
        free(t->account);
        free(t->memo);
        free(t);
        t = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <journalfile>\n", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("fopen");
        return 1;
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char account[256], memo[1024];
        int amount;
        int n = sscanf(line, "%255s %d %[^\n]", account, &amount, memo);
        if (n == 3) {
            add_transaction(account, amount, memo);
        }
    }
    free(line);
    fclose(f);

    // stdin commands
    line = NULL;
    len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[1024];
        int n = sscanf(line, "%15s %255s %255s %[^\n]", cmd, arg1, arg2, arg3);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 4) {
            int amount = atoi(arg2);
            add_transaction(arg1, amount, arg3);
        } else if (strcmp(cmd, "REVERSE") == 0 && n == 3) {
            int amount = atoi(arg2);
            reverse_transaction(arg1, amount);
        } else if (strcmp(cmd, "BALANCE") == 0 && n == 2) {
            printf("%d\n", balance_account(arg1));
        } else if (strcmp(cmd, "REPORT") == 0 && n == 1) {
            report_all();
        }
    }
    free(line);
    free_all();
    return 0;
}