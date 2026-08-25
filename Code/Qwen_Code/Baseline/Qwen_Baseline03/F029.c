// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char key[100];
    char val[1000];
} Rec;

Rec recs[1000];
int rn = 0;

int main() {
    char cmd[20], key[100], val[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            val[strcspn(val, "\n")] = 0;
            strcpy(recs[rn].key, key);
            strcpy(recs[rn].val, val);
            rn++;
        } else if (!strcmp(cmd, "DEDUP")) {
            for (int i = 0; i < rn; i++) {
                for (int j = i + 1; j < rn; ) {
                    if (!strcmp(recs[i].key, recs[j].key)) {
                        for (int k = j; k < rn - 1; k++) {
                            recs[k] = recs[k + 1];
                        }
                        rn--;
                    } else {
                        j++;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", key);
            for (int i = 0; i < rn; ) {
                if (!strcmp(recs[i].key, key)) {
                    for (int j = i; j < rn - 1; j++) {
                        recs[j] = recs[j + 1];
                    }
                    rn--;
                } else {
                    i++;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", key);
            for (int i = 0; i < rn; i++) {
                if (!strcmp(recs[i].key, key)) {
                    printf("%s\n", recs[i].val);
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < rn; i++) {
                printf("%s %s\n", recs[i].key, recs[i].val);
            }
        }
    }
    return 0;
}