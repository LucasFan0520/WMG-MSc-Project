// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Account {
    char *name;
    int balance;
    struct Account *next;
} Account;

Account *accounts = NULL;

Account* find_account(const char *name) {
    for (Account *a = accounts; a; a = a->next)
        if (strcmp(a->name, name) == 0) return a;
    return NULL;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char name[256], memo[256];
        int amount;
        if (sscanf(line, "%255s %d %255s", name, &amount, memo) == 3) {
            Account *a = find_account(name);
            if (!a) {
                a = malloc(sizeof(Account));
                if (!a) continue;
                a->name = strdup(name);
                a->balance = 0;
                a->next = accounts;
                accounts = a;
            }
            a->balance += amount;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], memo[256];
            int amount;
            if (sscanf(rest, "%255s %d %255s", name, &amount, memo) != 3) continue;
            Account *a = find_account(name);
            if (!a) {
                a = malloc(sizeof(Account));
                if (!a) continue;
                a->name = strdup(name);
                a->balance = 0;
                a->next = accounts;
                accounts = a;
            }
            a->balance += amount;
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char name[256];
            int amount;
            if (sscanf(rest, "%255s %d", name, &amount) != 2) continue;
            Account *a = find_account(name);
            if (a) a->balance -= amount;
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Account *a = find_account(name);
            printf("%d\n", a ? a->balance : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Account *a = accounts; a; a = a->next)
                printf("%s %d\n", a->name, a->balance);
        }
    }
    while (accounts) {
        Account *a = accounts;
        accounts = accounts->next;
        free(a->name);
        free(a);
    }
    return 0;
}