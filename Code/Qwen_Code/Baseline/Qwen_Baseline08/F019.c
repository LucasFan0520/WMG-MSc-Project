// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TRANS 10000
#define MAX_ACC 1000

typedef struct {
    char acc[100];
    int amount;
    char memo[100];
} Trans;

Trans trans[MAX_TRANS];
int tcount = 0;

typedef struct {
    char acc[100];
    int balance;
} Acc;

Acc accs[MAX_ACC];
int acount = 0;

int find_acc(const char *acc) {
    for (int i = 0; i < acount; i++) if (strcmp(accs[i].acc, acc) == 0) return i;
    return -1;
}

void add_acc(const char *acc, int amount) {
    int idx = find_acc(acc);
    if (idx == -1 && acount < MAX_ACC) {
        strcpy(accs[acount].acc, acc);
        accs[acount].balance = amount;
        acount++;
    } else if (idx != -1) {
        accs[idx].balance += amount;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char acc[100], memo[100];
            int amount;
            while (fscanf(f, "%s %d", acc, &amount) == 2) {
                fgets(memo, sizeof(memo), f);
                memo[strcspn(memo, "\n")] = 0;
                if (memo[0] == ' ') memmove(memo, memo + 1, strlen(memo));
                add_acc(acc, amount);
                if (tcount < MAX_TRANS) {
                    strcpy(trans[tcount].acc, acc);
                    trans[tcount].amount = amount;
                    strcpy(trans[tcount].memo, memo);
                    tcount++;
                }
            }
            fclose(f);
        }
    }
    char cmd[20], acc[100], memo[100];
    int amount;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            if (scanf("%s %d", acc, &amount) == 2) {
                fgets(memo, sizeof(memo), stdin);
                memo[strcspn(memo, "\n")] = 0;
                if (memo[0] == ' ') memmove(memo, memo + 1, strlen(memo));
                add_acc(acc, amount);
                if (tcount < MAX_TRANS) {
                    strcpy(trans[tcount].acc, acc);
                    trans[tcount].amount = amount;
                    strcpy(trans[tcount].memo, memo);
                    tcount++;
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (scanf("%s %d", acc, &amount) == 2) {
                add_acc(acc, -amount);
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            scanf("%s", acc);
            int idx = find_acc(acc);
            printf("%d\n", idx != -1 ? accs[idx].balance : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < acount; i++) printf("%s %d\n", accs[i].acc, accs[i].balance);
        }
    }
    return 0;
}