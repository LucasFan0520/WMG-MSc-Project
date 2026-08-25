// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    double amount;
    char *memo;
} Transaction;

int main(int argc, char *argv[]) {
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
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char acc[256], mem[256];
            double amt;
            if (sscanf(ptr, "%255s %lf %255s", acc, &amt, mem) == 3) {
                journal = realloc(journal, (count + 1) * sizeof(Transaction));
                journal[count].account = strdup(acc);
                journal[count].amount = amt;
                journal[count].memo = strdup(mem);
                count++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char acc[256];
            double amt;
            if (sscanf(ptr, "%255s %lf", acc, &amt) == 2) {
                journal = realloc(journal, (count + 1) * sizeof(Transaction));
                journal[count].account = strdup(acc);
                journal[count].amount = -amt;
                journal[count].memo = strdup("REVERSE");
                count++;
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char acc[256];
            if (sscanf(ptr, "%255s", acc) > 0) {
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
