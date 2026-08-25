// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Transaction {
    char *account;
    int amount; // can be negative?
    char *memo;
    struct Transaction *next;
} Transaction;

Transaction *head = NULL;

void free_trans(Transaction *t) {
    free(t->account);
    free(t->memo);
    free(t);
}

int is_valid_amount(const char *s) {
    if (!s || *s == '\0') return 0;
    for (; *s; s++) if (!isdigit(*s) && *s != '-') return 0;
    return 1;
}

void add_trans(const char *account, int amount, const char *memo) {
    Transaction *new = malloc(sizeof(Transaction));
    new->account = strdup(account);
    new->amount = amount;
    new->memo = strdup(memo);
    new->next = NULL;
    if (!head) head = new;
    else {
        Transaction *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void reverse_trans(const char *account, int amount) {
    // Add a negative transaction
    add_trans(account, -amount, "REVERSE");
}

int balance(const char *account) {
    int bal = 0;
    Transaction *t = head;
    while (t) {
        if (strcmp(t->account, account) == 0) bal += t->amount;
        t = t->next;
    }
    return bal;
}

void report() {
    // Aggregate balances per account
    // Since we have only a list, we can just accumulate unique accounts.
    // We'll use a simple array of strings.
    char **accounts = NULL;
    int *bals = NULL;
    int count = 0;
    Transaction *t = head;
    while (t) {
        int found = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(accounts[i], t->account) == 0) {
                bals[i] += t->amount;
                found = 1;
                break;
            }
        }
        if (!found) {
            accounts = realloc(accounts, (count+1)*sizeof(char*));
            bals = realloc(bals, (count+1)*sizeof(int));
            accounts[count] = strdup(t->account);
            bals[count] = t->amount;
            count++;
        }
        t = t->next;
    }
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", accounts[i], bals[i]);
        free(accounts[i]);
    }
    free(accounts);
    free(bals);
}

int main(int argc, char **argv) {
    if (argc == 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char *account = strtok(line, " ");
                char *amtstr = strtok(NULL, " ");
                char *memo = strtok(NULL, "");
                if (account && amtstr && memo) {
                    if (is_valid_amount(amtstr)) {
                        int amt = atoi(amtstr);
                        add_trans(account, amt, memo);
                    }
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *account = strtok(NULL, " ");
            char *amtstr = strtok(NULL, " ");
            char *memo = strtok(NULL, "");
            if (account && amtstr && memo) {
                if (is_valid_amount(amtstr)) {
                    int amt = atoi(amtstr);
                    add_trans(account, amt, memo);
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *account = strtok(NULL, " ");
            char *amtstr = strtok(NULL, " ");
            if (account && amtstr && is_valid_amount(amtstr)) {
                int amt = atoi(amtstr);
                reverse_trans(account, amt);
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *account = strtok(NULL, " ");
            if (account) {
                int bal = balance(account);
                printf("%d\n", bal);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    while (head) {
        Transaction *tmp = head;
        head = head->next;
        free_trans(tmp);
    }
    return 0;
}