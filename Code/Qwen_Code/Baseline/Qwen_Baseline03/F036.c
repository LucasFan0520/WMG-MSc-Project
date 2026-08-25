// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    char data[1000];
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
    char cmd[20], name[100], raw[1100];
    int len;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s", name, raw);
            char *colon = strchr(raw, ':');
            if (colon) {
                *colon = 0;
                len = atoi(raw);
                if (len == (int)strlen(colon + 1)) {
                    strcpy(recs[rn].name, name);
                    strcpy(recs[rn].data, colon + 1);
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
        } else if (!strcmp(cmd, "SHOW")) {
            scanf("%s", name);
            int i = findRec(name);
            if (i != -1) {
                for (char *p = recs[i].data; *p; p++) {
                    printf("%c", *p == ' ' ? '_' : *p);
                }
                printf("\n");
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < rn; i++) {
                printf("%s\n", recs[i].name);
            }
        }
    }
    return 0;
}