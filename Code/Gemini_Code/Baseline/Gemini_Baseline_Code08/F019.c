// F019.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

Transaction *journal = NULL;
int count = 0;
int capacity = 0;

void add_tx(const char *acc, double amt, const char *memo) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        journal = realloc(journal, capacity * sizeof(Transaction));
    }
    journal[count].account = strdup(acc);
    journal[count].amount = amt;
    journal[count].memo = strdup(memo);
    count++;
}

void balance_acc(const char *acc) {
    double bal = 0.0;
    for (int i = 0; i < count; i++) {
        if (strcmp(journal[i].account, acc) == 0) {
            bal += journal[i].amount;
        }
    }
    printf("%.2f\n", bal);
}

void report_journal(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %.2f %s\n", journal[i].account, journal[i].amount, journal[i].memo);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char acc[256], memo[256];
            double amt;
            while (fscanf(fp, "%255s %lf %255s", acc, &amt, memo) == 3) {
                add_tx(acc, amt, memo);
            }
            fclose(fp);
        }
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *acc = strtok(NULL, " ");
            char *amt_s = strtok(NULL, " ");
            char *memo = strtok(NULL, " ");
            if (acc && amt_s && memo) {
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (*endptr == '\0') {
                    add_tx(acc, amt, memo);
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = strtok(NULL, " ");
            char *amt_s = strtok(NULL, " ");
            if (acc && amt_s) {
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (*endptr == '\0') {
                    add_tx(acc, -amt, "REVERSE");
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = strtok(NULL, " ");
            if (acc) balance_acc(acc);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_journal();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(journal[i].account);
        free(journal[i].memo);
    }
    free(journal);
    return 0;
}
