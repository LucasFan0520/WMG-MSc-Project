// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *account;
    int amount;
    char *memo;
} Trans;

Trans *trans = NULL;
int tcount = 0;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1000];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char acc[100], memo[1000];
                int amt;
                if (sscanf(line, "%s %d %[^\n]", acc, &amt, memo) == 3) {
                    trans = realloc(trans, sizeof(Trans) * (tcount + 1));
                    trans[tcount].account = strdup(acc);
                    trans[tcount].amount = amt;
                    trans[tcount].memo = strdup(memo);
                    tcount++;
                }
            }
            fclose(f);
        }
    }
    
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char acc[100], memo[1000];
            int amt;
            if (scanf("%s %d", acc, &amt) == 2) {
                scanf(" %[^\n]", memo);
                trans = realloc(trans, sizeof(Trans) * (tcount + 1));
                trans[tcount].account = strdup(acc);
                trans[tcount].amount = amt;
                trans[tcount].memo = strdup(memo);
                tcount++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char acc[100];
            int amt;
            scanf("%s %d", acc, &amt);
            for (int i = tcount - 1; i >= 0; i--) {
                if (strcmp(trans[i].account, acc) == 0 && trans[i].amount == amt) {
                    free(trans[i].account);
                    free(trans[i].memo);
                    for (int j = i; j < tcount - 1; j++) trans[j] = trans[j+1];
                    tcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char acc[100];
            scanf("%s", acc);
            int bal = 0;
            for (int i = 0; i < tcount; i++) {
                if (strcmp(trans[i].account, acc) == 0) {
                    bal += trans[i].amount;
                }
            }
            printf("%d\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < tcount; i++) {
                printf("%s %d %s\n", trans[i].account, trans[i].amount, trans[i].memo);
            }
        }
    }
    for (int i = 0; i < tcount; i++) {
        free(trans[i].account);
        free(trans[i].memo);
    }
    free(trans);
    return 0;
}