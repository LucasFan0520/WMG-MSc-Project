// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char **argv) {
    Transaction *journal = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char acc[256], mem[256];
            double amt;
            while (fscanf(f, "%255s %lf %255s", acc, &amt, mem) == 3) {
                journal = realloc(journal, (count + 1) * sizeof(Transaction));
                journal[count].account = strdup(acc);
                journal[count].amount = amt;
                journal[count].memo = strdup(mem);
                count++;
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *acc = strtok(NULL, " ");
            char *amt_s = strtok(NULL, " ");
            char *mem = strtok(NULL, " ");
            if (acc && amt_s && mem) {
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (*endptr == '\0' || *endptr == '\r' || *endptr == '\n' || endptr != amt_s) {
                    journal = realloc(journal, (count + 1) * sizeof(Transaction));
                    journal[count].account = strdup(acc);
                    journal[count].amount = amt;
                    journal[count].memo = strdup(mem);
                    count++;
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *acc = strtok(NULL, " ");
            char *amt_s = strtok(NULL, " ");
            if (acc && amt_s) {
                char *endptr;
                double amt = strtod(amt_s, &endptr);
                if (*endptr == '\0' || *endptr == '\r' || *endptr == '\n' || endptr != amt_s) {
                    journal = realloc(journal, (count + 1) * sizeof(Transaction));
                    journal[count].account = strdup(acc);
                    journal[count].amount = -amt;
                    journal[count].memo = strdup("REVERSAL");
                    count++;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *acc = strtok(NULL, " ");
            if (acc) {
                double bal = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(journal[i].account, acc) == 0) {
                        bal += journal[i].amount;
                    }
                }
                printf("%.2f\n", bal);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %.2f %s\n", journal[i].account, journal[i].amount, journal[i].memo);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(journal[i].account);
        free(journal[i].memo);
    }
    free(journal);
    return 0;
}
