/* F019.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct acc {
    char *name;
    int balance;
    struct acc *next;
} Acc;

Acc *head = NULL;

Acc *find_acc(const char *name) {
    Acc *a = head;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void add_transaction(const char *acc, int amount, const char *memo) {
    Acc *a = find_acc(acc);
    if (!a) {
        a = malloc(sizeof(Acc));
        a->name = strdup(acc);
        a->balance = 0;
        a->next = head;
        head = a;
    }
    a->balance += amount;
    // memo ignored for balance, but we store? Not required.
}

void reverse_transaction(const char *acc, int amount) {
    Acc *a = find_acc(acc);
    if (a) a->balance -= amount;
}

void balance(const char *acc) {
    Acc *a = find_acc(acc);
    printf("%d\n", a ? a->balance : 0);
}

void report() {
    Acc *a = head;
    while (a) {
        printf("%s %d\n", a->name, a->balance);
        a = a->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char acc[256], memo[1024];
                int amount;
                if (sscanf(line, "%255s %d", acc, &amount) == 2) {
                    // memo may follow, we ignore but need to parse rest
                    char *rest = line + strlen(acc) + 1 + (amount >= 0 ? snprintf(NULL,0,"%d",amount) : 0) + 1;
                    if (*rest == ' ') rest++;
                    // we don't use memo for balance
                    add_transaction(acc, amount, rest);
                }
            }
            fclose(f);
        }
    }

    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], acc[256], memo[1024];
        int amount;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "REPORT") == 0) { report(); continue; }
        if (strcmp(cmd, "BALANCE") == 0) {
            if (sscanf(line, "%*s %255s", acc) == 1) balance(acc);
            continue;
        }
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %255s %d", acc, &amount) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(acc) + 1 + (amount >= 0 ? snprintf(NULL,0,"%d",amount) : 0) + 1;
            if (*rest == ' ') rest++;
            if (strcmp(cmd, "ADD") == 0) add_transaction(acc, amount, rest);
            else reverse_transaction(acc, amount);
        }
    }
    return 0;
}