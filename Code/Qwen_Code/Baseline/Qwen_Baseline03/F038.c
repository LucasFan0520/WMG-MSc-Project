// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[100];
    char enc[1000];
} RLE;

RLE rles[1000];
int rn = 0;

int findRLE(char *name) {
    for (int i = 0; i < rn; i++) {
        if (!strcmp(rles[i].name, name)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[100], enc[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "STORE")) {
            scanf("%s %s", name, enc);
            strcpy(rles[rn].name, name);
            strcpy(rles[rn].enc, enc);
            rn++;
        } else if (!strcmp(cmd, "DECODE")) {
            scanf("%s", name);
            int i = findRLE(name);
            if (i != -1) {
                char *p = rles[i].enc;
                while (*p) {
                    int count = 0;
                    while (*p >= '0' && *p <= '9') {
                        count = count * 10 + (*p - '0');
                        if (count > 100000) {
                            count = -1;
                            break;
                        }
                        p++;
                    }
                    if (count == -1 || !*p) break;
                    char c = *p++;
                    if (count > 1000) count = 1000;
                    for (int j = 0; j < count; j++) {
                        printf("%c", c == ' ' ? '_' : c);
                    }
                }
                printf("\n");
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int i = findRLE(name);
            if (i != -1) {
                for (int j = i; j < rn - 1; j++) {
                    rles[j] = rles[j + 1];
                }
                rn--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < rn; i++) {
                printf("%s\n", rles[i].name);
            }
        }
    }
    return 0;
}