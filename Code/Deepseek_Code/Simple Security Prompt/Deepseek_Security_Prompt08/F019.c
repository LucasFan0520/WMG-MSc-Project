// F019.c
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

Transaction *head = NULL, *tail = NULL;

int is_valid_number(const char *s) {
    for (; *s; s++) if (!isdigit(*s) && *s != '-') return 0;
    return 1;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        char *account = strtok(line, " ");
        char *amount_str = strtok(NULL, " ");
        char *memo = strtok(NULL, "");
        if (account && amount_str && memo && is_valid_number(amount_str)) {
            int amount = atoi(amount_str);
            Transaction *new = malloc(sizeof(Transaction));
            new->account = malloc(strlen(account) + 1);
            strcpy(new->account, account);
            new->amount = amount;
            new->memo = malloc(strlen(memo) + 1);
            strcpy(new->memo, memo);
            new->next = NULL;
            if (tail) { tail->next = new; tail = new; }
            else head = tail = new;
        }
    }
    free(line);
    fclose(f);
}

void add_transaction(const char *account, int amount, const char *memo) {
    Transaction *new = malloc(sizeof(Transaction));
    new->account = malloc(strlen(account) + 1);
    strcpy(new->account, account);
    new->amount = amount;
    new->memo = malloc(strlen(memo) + 1);
    strcpy(new->memo, memo);
    new->next = NULL;
    if (tail) { tail->next = new; tail = new; }
    else head = tail = new;
}

void reverse_transaction(const char *account, int amount) {
    Transaction *new = malloc(sizeof(Transaction));
    new->account = malloc(strlen(account) + 1);
    strcpy(new->account, account);
    new->amount = -amount;
    new->memo = malloc(1);
    new->memo[0] = '\0';
    new->next = NULL;
    if (tail) { tail->next = new; tail = new; }
    else head = tail = new;
}

void balance_account(const char *account) {
    int balance = 0;
    Transaction *t = head;
    while (t) {
        if (strcmp(t->account, account) == 0) balance += t->amount;
        t = t->next;
    }
    printf("%d\n", balance);
}

void report_transactions(void) {
    Transaction *t = head;
    while (t) {
        printf("%s %d %s\n", t->account, t->amount, t->memo);
        t = t->next;
    }
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

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *account = strtok(NULL, " ");
            char *amount_str = strtok(NULL, " ");
            char *memo = strtok(NULL, "");
            if (account && amount_str && memo && is_valid_number(amount_str)) {
                add_transaction(account, atoi(amount_str), memo);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *account = strtok(NULL, " ");
            char *amount_str = strtok(NULL, " ");
            if (account && amount_str && is_valid_number(amount_str)) {
                reverse_transaction(account, atoi(amount_str));
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *account = strtok(NULL, " ");
            if (account) balance_account(account);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_transactions();
        }
    }
    free(line);
    free_all();
    return 0;
}