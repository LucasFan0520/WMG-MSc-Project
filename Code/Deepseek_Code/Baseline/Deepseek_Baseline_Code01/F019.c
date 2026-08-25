// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct acc {
    char *name;
    int balance;
    struct acc *next;
} Acc;

Acc *head = NULL;
Acc *tail = NULL;

Acc *find_acc(const char *name) {
    Acc *a = head;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void add_transaction(const char *name, int amount) {
    Acc *a = find_acc(name);
    if (!a) {
        a = malloc(sizeof(Acc));
        a->name = strdup(name);
        a->balance = 0;
        a->next = NULL;
        if (tail) tail->next = a;
        else head = a;
        tail = a;
    }
    a->balance += amount;
}

void reverse_transaction(const char *name, int amount) {
    Acc *a = find_acc(name);
    if (a) a->balance -= amount;
}

void balance_acc(const char *name) {
    Acc *a = find_acc(name);
    printf("%d\n", a ? a->balance : 0);
}

void report_acc(void) {
    Acc *a = head;
    while (a) {
        printf("%s %d\n", a->name, a->balance);
        a = a->next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[256], memo[256];
        int amount;
        if (sscanf(line, "%255s %d %255s", name, &amount, memo) == 3) {
            add_transaction(name, amount);
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], memo[256];
        int amount;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d %255s", name, &amount, memo) == 3)
                add_transaction(name, amount);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %255s %d", name, &amount) == 2)
                reverse_transaction(name, amount);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                balance_acc(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_acc();
        }
    }
    return 0;
}