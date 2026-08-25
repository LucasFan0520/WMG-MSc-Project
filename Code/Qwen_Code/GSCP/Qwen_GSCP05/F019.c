// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *acc; int amt; char *memo; } Tx;
Tx *txs = NULL; int tc = 0;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char acc[256], memo[700]; int amt;
                if (sscanf(line, "%255s %d %700[^\n]", acc, &amt, memo) == 3) {
                    txs = realloc(txs, (tc+1)*sizeof(Tx));
                    txs[tc].acc = strdup(acc); txs[tc].amt = amt; txs[tc].memo = strdup(memo); tc++;
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c4[700]; int amt;
        c2[0] = c4[0] = 0;
        sscanf(line, "%19s %255s %d %700[^\n]", c1, c2, &amt, c4);
        
        if (strcmp(c1, "ADD") == 0) {
            txs = realloc(txs, (tc+1)*sizeof(Tx));
            txs[tc].acc = strdup(c2); txs[tc].amt = amt; txs[tc].memo = strdup(c4); tc++;
        } else if (strcmp(c1, "REVERSE") == 0) {
            for (int i = 0; i < tc; ) {
                if (strcmp(txs[i].acc, c2) == 0 && txs[i].amt == amt) {
                    free(txs[i].acc); free(txs[i].memo);
                    for (int j = i; j < tc-1; j++) txs[j] = txs[j+1];
                    tc--;
                } else i++;
            }
        } else if (strcmp(c1, "BALANCE") == 0) {
            int bal = 0;
            for (int i = 0; i < tc; i++) if (strcmp(txs[i].acc, c2) == 0) bal += txs[i].amt;
            printf("%d\n", bal);
        } else if (strcmp(c1, "REPORT") == 0) {
            for (int i = 0; i < tc; i++) printf("%s %d %s\n", txs[i].acc, txs[i].amt, txs[i].memo);
        }
    }
    for (int i = 0; i < tc; i++) { free(txs[i].acc); free(txs[i].memo); }
    free(txs);
    return 0;
}