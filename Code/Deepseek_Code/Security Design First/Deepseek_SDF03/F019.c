// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_ACCOUNT 64
#define MAX_MEMO 256

typedef struct Transaction {
    char account[MAX_ACCOUNT];
    int amount;
    char memo[MAX_MEMO];
    struct Transaction *next;
} Transaction;

Transaction *head = NULL;

void free_all() {
    Transaction *t = head;
    while (t) {
        Transaction *tmp = t;
        t = t->next;
        free(tmp);
    }
    head = NULL;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char account[MAX_ACCOUNT], memo[MAX_MEMO];
        int amount;
        if (sscanf(line, "%63s %d %255s", account, &amount, memo) == 3) {
            Transaction *t = malloc(sizeof(Transaction));
            if (!t) continue;
            strcpy(t->account, account);
            t->amount = amount;
            strcpy(t->memo, memo);
            t->next = head;
            head = t;
        }
    }
    fclose(f);
}

void handle_add(char *line) {
    char account[MAX_ACCOUNT], memo[MAX_MEMO];
    int amount;
    if (sscanf(line, "ADD %63s %d %255s", account, &amount, memo) != 3) return;
    Transaction *t = malloc(sizeof(Transaction));
    if (!t) return;
    strcpy(t->account, account);
    t->amount = amount;
    strcpy(t->memo, memo);
    t->next = head;
    head = t;
}

void handle_reverse(char *line) {
    char account[MAX_ACCOUNT];
    int amount;
    if (sscanf(line, "REVERSE %63s %d", account, &amount) != 2) return;
    Transaction *t = malloc(sizeof(Transaction));
    if (!t) return;
    strcpy(t->account, account);
    t->amount = -amount;
    strcpy(t->memo, "REVERSE");
    t->next = head;
    head = t;
}

void handle_balance(char *line) {
    char account[MAX_ACCOUNT];
    if (sscanf(line, "BALANCE %63s", account) != 1) return;
    int bal = 0;
    Transaction *t = head;
    while (t) {
        if (strcmp(t->account, account) == 0) {
            bal += t->amount;
        }
        t = t->next;
    }
    printf("%d\n", bal);
}

void handle_report() {
    Transaction *t = head;
    while (t) {
        printf("%s %d %s\n", t->account, t->amount, t->memo);
        t = t->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "REVERSE ", 8) == 0) handle_reverse(line);
        else if (strncmp(line, "BALANCE ", 8) == 0) handle_balance(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}