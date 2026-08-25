// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char key[100];
    char val[1000];
} KV;

KV committed[1000];
int cn = 0;
KV staged[1000];
int sn = 0;
int in_trans = 0;

int findC(char *key) {
    for (int i = 0; i < cn; i++) {
        if (!strcmp(committed[i].key, key)) return i;
    }
    return -1;
}

int findS(char *key) {
    for (int i = 0; i < sn; i++) {
        if (!strcmp(staged[i].key, key)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], key[100], val[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "SET")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            val[strcspn(val, "\n")] = 0;
            int i = findC(key);
            if (i != -1) {
                strcpy(committed[i].val, val);
            } else {
                strcpy(committed[cn].key, key);
                strcpy(committed[cn].val, val);
                cn++;
            }
        } else if (!strcmp(cmd, "BEGIN")) {
            in_trans = 1;
            sn = 0;
        } else if (!strcmp(cmd, "TSET")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            val[strcspn(val, "\n")] = 0;
            int i = findS(key);
            if (i != -1) {
                strcpy(staged[i].val, val);
            } else {
                strcpy(staged[sn].key, key);
                strcpy(staged[sn].val, val);
                sn++;
            }
        } else if (!strcmp(cmd, "COMMIT")) {
            for (int i = 0; i < sn; i++) {
                int j = findC(staged[i].key);
                if (j != -1) {
                    strcpy(committed[j].val, staged[i].val);
                } else {
                    strcpy(committed[cn].key, staged[i].key);
                    strcpy(committed[cn].val, staged[i].val);
                    cn++;
                }
            }
            sn = 0;
            in_trans = 0;
        } else if (!strcmp(cmd, "ABORT")) {
            sn = 0;
            in_trans = 0;
        } else if (!strcmp(cmd, "GET")) {
            scanf("%s", key);
            int i = findC(key);
            if (i != -1) {
                printf("%s\n", committed[i].val);
            }
        } else if (!strcmp(cmd, "DUMP")) {
            for (int i = 0; i < cn; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].val);
            }
        }
    }
    return 0;
}