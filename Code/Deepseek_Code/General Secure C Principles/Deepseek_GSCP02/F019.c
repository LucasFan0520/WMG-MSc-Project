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

Account *head = NULL;

Account *find_account(const char *name) {
    Account *a = head;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void add_transaction(const char *name, int amount) {
    Account *a = find_account(name);
    if (a) {
        a->balance += amount;
    } else {
        a = malloc(sizeof(Account));
        if (!a) return;
        a->name = strdup(name);
        a->balance = amount;
        a->next = head;
        head = a;
    }
}

void reverse_transaction(const char *name, int amount) {
    add_transaction(name, -amount);
}

void balance_account(const char *name) {
    Account *a = find_account(name);
    printf("%d\n", a ? a->balance : 0);
}

void report_all(void) {
    Account *a = head;
    while (a) {
        printf("%s %d\n", a->name, a->balance);
        a = a->next;
    }
}

void free_all(void) {
    while (head) {
        Account *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}

int is_number(const char *s) {
    while (*s) {
        if (!isdigit(*s) && *s != '-') return 0;
        s++;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char account[256], amount_str[256], memo[256];
                if (sscanf(line, "%255s %255s %255s", account, amount_str, memo) == 3) {
                    if (is_number(amount_str)) {
                        int amt = atoi(amount_str);
                        add_transaction(account, amt);
                    }
                }
            }
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char account[256], amount_str[256], memo[256];
            if (sscanf(line, "%*s %255s %255s %255s", account, amount_str, memo) == 3) {
                if (is_number(amount_str)) {
                    int amt = atoi(amount_str);
                    add_transaction(account, amt);
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char account[256], amount_str[256];
            if (sscanf(line, "%*s %255s %255s", account, amount_str) == 2) {
                if (is_number(amount_str)) {
                    int amt = atoi(amount_str);
                    reverse_transaction(account, amt);
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char account[256];
            if (sscanf(line, "%*s %255s", account) == 1) {
                balance_account(account);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_all();
        }
    }

    free(line);
    free_all();
    return 0;
}