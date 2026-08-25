// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double balance;
} AccountBal;

AccountBal *journal = NULL;
int count = 0;
int capacity = 0;

int find_account(const char *account) {
    for (int i = 0; i < count; i++) {
        if (strcmp(journal[i].account, account) == 0) {
            return i;
        }
    }
    return -1;
}

void update_balance(const char *account, const char *amount_str, int is_reverse) {
    char *endptr;
    double val = strtod(amount_str, &endptr);
    if (endptr == amount_str || *endptr != '\0') {
        return;
    }
    if (is_reverse) {
        val = -val;
    }
    int idx = find_account(account);
    if (idx != -1) {
        journal[idx].balance += val;
    } else {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            journal = realloc(journal, capacity * sizeof(AccountBal));
        }
        journal[count].account = strdup(account);
        journal[count].balance = val;
        count++;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char acc[256], amt[256], mem[256];
            while (fscanf(f, "%255s %255s %255s", acc, amt, mem) == 3) {
                update_balance(acc, amt, 0);
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                char *p3 = strchr(p2 + 1, ' ');
                if (p3) {
                    *p3 = 0;
                    update_balance(p1, p2 + 1, 0);
                }
            }
        } else if (strncmp(line, "REVERSE ", 8) == 0) {
            char *p1 = line + 8;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                update_balance(p1, p2 + 1, 1);
            }
        } else if (strncmp(line, "BALANCE ", 8) == 0) {
            char *acc = line + 8;
            int idx = find_account(acc);
            if (idx != -1) {
                printf("%.2f\n", journal[idx].balance);
            } else {
                printf("0.00\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f\n", journal[i].account, journal[i].balance);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(journal[i].account);
    }
    free(journal);
    return 0;
}
