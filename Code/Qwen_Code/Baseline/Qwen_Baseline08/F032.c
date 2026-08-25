// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SET 1000
#define MAX_LEN 1000

typedef struct {
    char key[100];
    char val[MAX_LEN];
} Setting;

Setting settings[MAX_SET];
int scount = 0;

Setting trans[MAX_SET];
int tcount = 0;
int in_trans = 0;

int find_set(const char *key) {
    for (int i = 0; i < scount; i++) if (strcmp(settings[i].key, key) == 0) return i;
    return -1;
}

int find_trans(const char *key) {
    for (int i = 0; i < tcount; i++) if (strcmp(trans[i].key, key) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], key[100], val[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key);
            fgets(val, MAX_LEN, stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            int idx = find_set(key);
            if (idx == -1 && scount < MAX_SET) {
                strcpy(settings[scount].key, key);
                strcpy(settings[scount].val, val);
                scount++;
            } else if (idx != -1) {
                strcpy(settings[idx].val, val);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
            tcount = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_trans) {
                scanf("%s", key);
                fgets(val, MAX_LEN, stdin);
                val[strcspn(val, "\n")] = 0;
                if (val[0] == ' ') memmove(val, val + 1, strlen(val));
                int idx = find_trans(key);
                if (idx == -1 && tcount < MAX_SET) {
                    strcpy(trans[tcount].key, key);
                    strcpy(trans[tcount].val, val);
                    tcount++;
                } else if (idx != -1) {
                    strcpy(trans[idx].val, val);
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                for (int i = 0; i < tcount; i++) {
                    int idx = find_set(trans[i].key);
                    if (idx == -1 && scount < MAX_SET) {
                        strcpy(settings[scount].key, trans[i].key);
                        strcpy(settings[scount].val, trans[i].val);
                        scount++;
                    } else if (idx != -1) {
                        strcpy(settings[idx].val, trans[i].val);
                    }
                }
                in_trans = 0;
                tcount = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            in_trans = 0;
            tcount = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s", key);
            int idx = find_set(key);
            if (idx != -1) printf("%s\n", settings[idx].val);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < scount; i++) {
                printf("%s=%s\n", settings[i].key, settings[i].val);
            }
        }
    }
    return 0;
}