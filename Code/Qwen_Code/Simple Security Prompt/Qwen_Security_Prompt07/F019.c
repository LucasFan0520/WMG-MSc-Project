// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *acc, *memo; int amt; } Trans;
Trans *trans = 0; int n = 0, cap = 0;

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = 0;
        char acc[256], memo[700]; int amt;
        memo[0] = 0;
        if (sscanf(line, "%s %d %[^\n]", acc, &amt, memo) >= 2) {
            if (n == cap) { cap = cap ? cap*2 : 4; trans = realloc(trans, cap*sizeof(Trans)); }
            trans[n].acc = strdup(acc); trans[n].amt = amt; trans[n].memo = strdup(memo); n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], acc[256], memo[700]; int amt;
        memo[0] = 0;
        sscanf(line, "%s %s %d %[^\n]", cmd, acc, &amt, memo);
        if (!strcmp(cmd, "ADD")) {
            if (n == cap) { cap = cap ? cap*2 : 4; trans = realloc(trans, cap*sizeof(Trans)); }
            trans[n].acc = strdup(acc); trans[n].amt = amt; trans[n].memo = strdup(memo); n++;
        } else if (!strcmp(cmd, "REVERSE")) {
            for (int i=n-1; i>=0; i--) {
                if (!strcmp(trans[i].acc, acc) && trans[i].amt == amt) {
                    trans[i].amt = -trans[i].amt; break;
                }
            }
        } else if (!strcmp(cmd, "BALANCE")) {
            int sum = 0;
            for (int i=0; i<n; i++) if (!strcmp(trans[i].acc, acc)) sum += trans[i].amt;
            printf("%d\n", sum);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i=0; i<n; i++) printf("%s %d %s\n", trans[i].acc, trans[i].amt, trans[i].memo);
        }
    }
    return 0;
}