// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ACCOUNT 100
#define MAX_MEMO 1000

typedef struct {
    char account[MAX_ACCOUNT];
    int amount;
    char memo[MAX_MEMO];
} Transaction;

Transaction transactions[10000];
int trans_count = 0;

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    char line[1200];
    while (fgets(line, sizeof(line), f)) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        
        char *account = strtok(line, " ");
        char *amount_str = strtok(NULL, " ");
        char *memo = strtok(NULL, "");
        
        if (account && amount_str) {
            char *endptr;
            long amount = strtol(amount_str, &endptr, 10);
            if (*endptr == '\0' || *endptr == ' ') {
                strcpy(transactions[trans_count].account, account);
                transactions[trans_count].amount = (int)amount;
                if (memo) {
                    if (memo[0] == ' ') memo++;
                    strcpy(transactions[trans_count].memo, memo);
                } else {
                    transactions[trans_count].memo[0] = '\0';
                }
                trans_count++;
            }
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char cmd[20], account[MAX_ACCOUNT], memo[MAX_MEMO];
    int amount;
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %d", account, &amount);
            fgets(memo, MAX_MEMO, stdin);
            if (memo[0] == ' ') memmove(memo, memo + 1, strlen(memo));
            if (memo[strlen(memo) - 1] == '\n') memo[strlen(memo) - 1] = '\0';
            
            strcpy(transactions[trans_count].account, account);
            transactions[trans_count].amount = amount;
            strcpy(transactions[trans_count].memo, memo);
            trans_count++;
        } else if (strcmp(cmd, "REVERSE") == 0) {
            scanf("%s %d", account, &amount);
            for (int i = trans_count - 1; i >= 0; i--) {
                if (strcmp(transactions[i].account, account) == 0 && 
                    transactions[i].amount == amount) {
                    for (int j = i; j < trans_count - 1; j++) {
                        transactions[j] = transactions[j + 1];
                    }
                    trans_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            scanf("%s", account);
            int balance = 0;
            for (int i = 0; i < trans_count; i++) {
                if (strcmp(transactions[i].account, account) == 0) {
                    balance += transactions[i].amount;
                }
            }
            printf("%d\n", balance);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < trans_count; i++) {
                printf("%s %d %s\n", transactions[i].account, 
                       transactions[i].amount, transactions[i].memo);
            }
        }
    }
    
    return 0;
}