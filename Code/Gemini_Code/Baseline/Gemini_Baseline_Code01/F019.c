// F019.c
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

void add_transaction(char *acc, double amt, char *mem) {
    journal = realloc(journal, (count + 1) * sizeof(Transaction));
    journal[count].account = strdup(acc);
    journal[count].amount = amt;
    journal[count].memo = strdup(mem);
    count++;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char acc[256], mem[256];
        double amt;
        while (fscanf(fp, "%255s %lf %255s", acc, &amt, mem) == 3) {
            add_transaction(acc, amt, mem);
        }
        fclose(fp);
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            char acc[256], mem[256];
            double amt;
            if (sscanf(args, "%255s %lf %255s", acc, &amt, mem) == 3) {
                add_transaction(acc, amt, mem);
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char acc[256];
            double amt;
            if (sscanf(args, "%255s %lf", acc, &amt) == 2) {
                add_transaction(acc, -amt, "REVERSE");
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char acc[256];
            if (sscanf(args, "%255s", acc) == 1) {
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
