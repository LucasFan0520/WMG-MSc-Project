// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *acc; int amt; char *memo; } Tx;
Tx tx[10000]; int nt = 0;

int find(char *acc) {
    for (int i = 0; i < nt; i++) if (!strcmp(tx[i].acc, acc)) return i;
    return -1;
}

void load(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *a = strtok(line, " ");
        char *am = strtok(NULL, " ");
        char *m = strtok(NULL, "");
        if (a && am && m) {
            while(*m==' ') m++;
            int amt = atoi(am);
            if (amt != 0 || strcmp(am, "0") == 0) {
                tx[nt].acc = strdup(a); tx[nt].amt = amt; tx[nt++].memo = strdup(m);
            }
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char cmd[20], acc[100], memo[1000]; int amt;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %d", acc, &amt); fgets(memo, sizeof(memo), stdin);
            memo[strcspn(memo, "\n")] = 0;
            if (memo[0] == ' ') memmove(memo, memo + 1, strlen(memo));
            tx[nt].acc = strdup(acc); tx[nt].amt = amt; tx[nt++].memo = strdup(memo);
        } else if (!strcmp(cmd, "REVERSE")) {
            scanf("%s %d", acc, &amt);
            for (int i = nt - 1; i >= 0; i--) {
                if (!strcmp(tx[i].acc, acc) && tx[i].amt == amt) {
                    free(tx[i].acc); free(tx[i].memo);
                    for (int j = i; j < nt - 1; j++) tx[j] = tx[j + 1];
                    nt--; break;
                }
            }
        } else if (!strcmp(cmd, "BALANCE")) {
            scanf("%s", acc); int bal = 0;
            for (int i = 0; i < nt; i++) if (!strcmp(tx[i].acc, acc)) bal += tx[i].amt;
            printf("%d\n", bal);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nt; i++) printf("%s %d %s\n", tx[i].acc, tx[i].amt, tx[i].memo);
        }
    }
    for (int i = 0; i < nt; i++) { free(tx[i].acc); free(tx[i].memo); }
    return 0;
}