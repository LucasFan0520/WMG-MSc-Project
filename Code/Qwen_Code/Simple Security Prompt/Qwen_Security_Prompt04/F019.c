// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *acc;
    int amt;
    char *memo;
} Trans;

Trans ts[10000];
int tc = 0;

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char acc[1000];
        int amt;
        char memo[10000] = "";
        if (sscanf(line, "%s %d %[^\n]", acc, &amt, memo) >= 2) {
            ts[tc].acc = strdup(acc);
            ts[tc].amt = amt;
            ts[tc].memo = strdup(memo);
            tc++;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char acc[1000];
            int amt;
            sscanf(rest, "%s %d", acc, &amt);
            char *memo = rest;
            while (*memo && *memo != ' ') memo++;
            while (*memo == ' ') memo++;
            while (*memo && *memo != ' ') memo++;
            while (*memo == ' ') memo++;
            ts[tc].acc = strdup(acc);
            ts[tc].amt = amt;
            ts[tc].memo = strdup(memo);
            tc++;
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char acc[1000];
            int amt;
            sscanf(rest, "%s %d", acc, &amt);
            for (int i = 0; i < tc; i++) {
                if (strcmp(ts[i].acc, acc) == 0 && ts[i].amt == amt) {
                    free(ts[i].acc); free(ts[i].memo);
                    for (int j = i; j < tc - 1; j++) ts[j] = ts[j+1];
                    tc--;
                    break;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char acc[1000];
            sscanf(rest, "%s", acc);
            int bal = 0;
            for (int i = 0; i < tc; i++) {
                if (strcmp(ts[i].acc, acc) == 0) bal += ts[i].amt;
            }
            printf("%d\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < tc; i++) printf("%s %d %s\n", ts[i].acc, ts[i].amt, ts[i].memo);
        }
    }
    for (int i = 0; i < tc; i++) {
        free(ts[i].acc); free(ts[i].memo);
    }
    return 0;
}