// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Trans {
    char *account;
    char *memo;
    int amount;
    struct Trans *next;
} Trans;

Trans *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char acc[100];
        char memo[800];
        int amt;
        if (sscanf(line, "%s %d %[^\n]", acc, &amt, memo) == 3) {
            Trans *p = malloc(sizeof(Trans));
            p->account = strdup(acc);
            p->amount = amt;
            p->memo = strdup(memo);
            p->next = head;
            head = p;
        }
    }
    fclose(f);
}

void add_trans(const char *acc, int amt, const char *memo) {
    Trans *p = malloc(sizeof(Trans));
    p->account = strdup(acc);
    p->amount = amt;
    p->memo = strdup(memo);
    p->next = head;
    head = p;
}

void reverse_trans(const char *acc, int amt) {
    for (Trans *p = head; p; p = p->next) {
        if (strcmp(p->account, acc) == 0 && p->amount == amt) {
            p->amount = -p->amount;
            return;
        }
    }
}

void balance_trans(const char *acc) {
    int bal = 0;
    for (Trans *p = head; p; p = p->next) {
        if (strcmp(p->account, acc) == 0) {
            bal += p->amount;
        }
    }
    printf("%d\n", bal);
}

void report_trans() {
    for (Trans *p = head; p; p = p->next) {
        printf("%s %d %s\n", p->account, p->amount, p->memo);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[1000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char acc[100];
        char memo[800];
        int amt;
        memo[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %d %[^\n]", acc, &amt, memo);
            add_trans(acc, amt, memo);
        } else if (strcmp(cmd, "REVERSE") == 0) {
            sscanf(line, "%*s %s %d", acc, &amt);
            reverse_trans(acc, amt);
        } else if (strcmp(cmd, "BALANCE") == 0) {
            sscanf(line, "%*s %s", acc);
            balance_trans(acc);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_trans();
        }
    }
    while (head) {
        Trans *n = head->next;
        free(head->account);
        free(head->memo);
        free(head);
        head = n;
    }
    return 0;
}