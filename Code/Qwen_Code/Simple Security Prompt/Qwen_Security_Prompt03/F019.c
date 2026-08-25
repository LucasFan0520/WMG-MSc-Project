// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *acc; int amt; char *memo; } Tx;
Tx *tx = NULL;
int n = 0;

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char acc[256], memo[800];
                int amt;
                if (sscanf(line, "%s %d", acc, &amt) == 2) {
                    char *m = strchr(line, ' '); m++; m = strchr(m, ' ');
                    tx = realloc(tx, (n + 1) * sizeof(Tx));
                    tx[n].acc = strdup(acc); tx[n].amt = amt; tx[n].memo = strdup(m ? m + 1 : ""); n++;
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], acc[256], memo[800];
        int amt;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %s %d", acc, &amt) == 2) {
                char *m = strchr(line, ' '); m++; m = strchr(m, ' '); m++;
                tx = realloc(tx, (n + 1) * sizeof(Tx));
                tx[n].acc = strdup(acc); tx[n].amt = amt; tx[n].memo = strdup(m ? m : ""); n++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %s %d", acc, &amt) == 2) {
                for (int i = n - 1; i >= 0; i--) {
                    if (strcmp(tx[i].acc, acc) == 0 && tx[i].amt == amt) {
                        tx[i].amt = -tx[i].amt; break;
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            sscanf(line, "%*s %s", acc);
            int bal = 0;
            for (int i = 0; i < n; i++) if (strcmp(tx[i].acc, acc) == 0) bal += tx[i].amt;
            printf("%d\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n; i++) printf("%s %d %s\n", tx[i].acc, tx[i].amt, tx[i].memo);
        }
    }
    for (int i = 0; i < n; i++) { free(tx[i].acc); free(tx[i].memo); }
    free(tx);
    return 0;
}