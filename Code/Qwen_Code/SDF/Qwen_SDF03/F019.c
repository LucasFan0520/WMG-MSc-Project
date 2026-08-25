// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Account {
    char *name;
    long balance;
    struct Account *next;
} Account;

Account *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Account *find_account(const char *name) {
    for (Account *a = head; a; a = a->next)
        if (strcmp(a->name, name) == 0) return a;
    return NULL;
}

void parse_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *ptr = line;
        char *acc = get_token(&ptr);
        char *amt_str = get_token(&ptr);
        if (acc && amt_str) {
            char *end;
            long amt = strtol(amt_str, &end, 10);
            if (*end == '\0') {
                Account *a = find_account(acc);
                if (!a) {
                    a = malloc(sizeof(Account));
                    if (a) { a->name = strdup(acc); a->balance = 0; a->next = head; head = a; }
                }
                if (a) a->balance += amt;
            }
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) parse_file(argv[1]);
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *acc = get_token(&ptr);
            char *amt_str = get_token(&ptr);
            if (acc && amt_str) {
                char *end;
                long amt = strtol(amt_str, &end, 10);
                if (*end == '\0' || *end == ' ') {
                    Account *a = find_account(acc);
                    if (!a) {
                        a = malloc(sizeof(Account));
                        if (a) { a->name = strdup(acc); a->balance = 0; a->next = head; head = a; }
                    }
                    if (a) a->balance += amt;
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = get_token(&ptr);
            char *amt_str = get_token(&ptr);
            if (acc && amt_str) {
                char *end;
                long amt = strtol(amt_str, &end, 10);
                if (*end == '\0') {
                    Account *a = find_account(acc);
                    if (a) a->balance -= amt;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = get_token(&ptr);
            if (acc) {
                Account *a = find_account(acc);
                printf("%ld\n", a ? a->balance : 0);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Account *a = head; a; a = a->next) printf("%s %ld\n", a->name, a->balance);
        }
    }
    while (head) {
        Account *next = head->next;
        free(head->name); free(head);
        head = next;
    }
    return 0;
}