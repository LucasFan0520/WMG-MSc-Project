// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *acc;
    char *memo;
    int amt;
} Trans;

Trans *trans = NULL;
int n = 0;

void load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char acc[100];
        int amt;
        if (sscanf(line, "%s %d", acc, &amt) == 2) {
            char *p = line;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            trans = realloc(trans, sizeof(Trans) * (n + 1));
            trans[n].acc = strdup(acc);
            trans[n].amt = amt;
            trans[n].memo = strdup(p);
            n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        int amt;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %s %d", arg1, &amt) == 2) {
                char *p = line;
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
                trans = realloc(trans, sizeof(Trans) * (n + 1));
                trans[n].acc = strdup(arg1);
                trans[n].amt = amt;
                trans[n].memo = strdup(p);
                n++;
            }
        } else if (strcmp(cmd, "REVERSE") == 0) {
            if (sscanf(line, "%*s %s %d", arg1, &amt) == 2) {
                for (int i = 0; i < n; ) {
                    if (strcmp(trans[i].acc, arg1) == 0 && trans[i].amt == amt) {
                        free(trans[i].acc);
                        free(trans[i].memo);
                        for (int j = i; j < n - 1; j++) trans[j] = trans[j + 1];
                        n--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            sscanf(line, "%*s %s", arg1);
            int sum = 0;
            for (int i = 0; i < n; i++)
                if (strcmp(trans[i].acc, arg1) == 0) sum += trans[i].amt;
            printf("%d\n", sum);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n; i++) printf("%s %d %s\n", trans[i].acc, trans[i].amt, trans[i].memo);
        }
    }
    for (int i = 0; i < n; i++) {
        free(trans[i].acc);
        free(trans[i].memo);
    }
    free(trans);
    return 0;
}