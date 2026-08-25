// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *acc;
    int amt;
    char *memo;
} Transaction;

Transaction *trans = NULL;
int count = 0;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *acc = strtok(line, " ");
        char *amt_s = strtok(NULL, " ");
        char *memo = strtok(NULL, "");
        if (acc && amt_s) {
            int amt = atoi(amt_s);
            if (amt != 0 || strcmp(amt_s, "0") == 0) {
                trans = realloc(trans, sizeof(Transaction) * (count + 1));
                trans[count].acc = strdup(acc);
                trans[count].amt = amt;
                trans[count].memo = memo ? strdup(memo) : strdup("");
                count++;
            }
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char acc[1000];
    char memo[10000];
    int amt;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %d", acc, &amt);
            fgets(memo, sizeof(memo), stdin);
            if (memo[0] == ' ') {
                memmove(memo, memo + 1, strlen(memo));
            }
            memo[strcspn(memo, "\n")] = 0;
            trans = realloc(trans, sizeof(Transaction) * (count + 1));
            trans[count].acc = strdup(acc);
            trans[count].amt = amt;
            trans[count].memo = strdup(memo);
            count++;
        } else if (strcmp(cmd, "REVERSE") == 0) {
            scanf("%s %d", acc, &amt);
            for (int i = count - 1; i >= 0; i--) {
                if (strcmp(trans[i].acc, acc) == 0 && trans[i].amt == amt) {
                    free(trans[i].acc);
                    free(trans[i].memo);
                    for (int j = i; j < count - 1; j++) {
                        trans[j] = trans[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            scanf("%s", acc);
            int bal = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(trans[i].acc, acc) == 0) {
                    bal += trans[i].amt;
                }
            }
            printf("%d\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", trans[i].acc, trans[i].amt, trans[i].memo);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(trans[i].acc);
        free(trans[i].memo);
    }
    free(trans);

    return 0;
}