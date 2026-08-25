// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    char note[1000];
} Rec;

Rec recs[1000];
int rn = 0;

int findRec(char *name) {
    for (int i = 0; i < rn; i++) {
        if (!strcmp(recs[i].name, name)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[100], note[1000];
    int n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "BATCH")) {
            scanf("%d", &n);
            Rec tmp[1000];
            int valid = 1;
            char line[1100];
            for (int i = 0; i < n; i++) {
                if (!fgets(line, 1100, stdin)) {
                    valid = 0;
                    break;
                }
                line[strcspn(line, "\n")] = 0;
                char *space = strchr(line, ' ');
                if (!space) {
                    valid = 0;
                    break;
                }
                *space = 0;
                if (strlen(line) >= 100) {
                    valid = 0;
                    break;
                }
                strcpy(tmp[i].name, line);
                strcpy(tmp[i].note, space + 1);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    strcpy(recs[rn].name, tmp[i].name);
                    strcpy(recs[rn].note, tmp[i].note);
                    rn++;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int i = findRec(name);
            if (i != -1) {
                for (int j = i; j < rn - 1; j++) {
                    recs[j] = recs[j + 1];
                }
                rn--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name);
            int i = findRec(name);
            if (i != -1) {
                printf("%s %s\n", recs[i].name, recs[i].note);
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < rn; i++) {
                printf("%s %s\n", recs[i].name, recs[i].note);
            }
        }
    }
    return 0;
}