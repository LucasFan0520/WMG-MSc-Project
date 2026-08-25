// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } Set;
Set committed[10000]; int ncommitted = 0;
Set staged[10000]; int nstaged = 0;
int in_transaction = 0;

int find(Set *arr, int n, char *key) {
    for (int i = 0; i < n; i++) if (!strcmp(arr[i].key, key)) return i;
    return -1;
}

int main() {
    char cmd[20], key[100], val[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "SET")) {
            scanf("%s", key);
            fgets(val, sizeof(val), stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            int idx = find(committed, ncommitted, key);
            if (idx != -1) { free(committed[idx].val); committed[idx].val = strdup(val); }
            else { committed[ncommitted].key = strdup(key); committed[ncommitted++].val = strdup(val); }
        } else if (!strcmp(cmd, "BEGIN")) {
            in_transaction = 1;
            nstaged = 0;
        } else if (!strcmp(cmd, "TSET")) {
            scanf("%s", key);
            fgets(val, sizeof(val), stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            int idx = find(staged, nstaged, key);
            if (idx != -1) { free(staged[idx].val); staged[idx].val = strdup(val); }
            else { staged[nstaged].key = strdup(key); staged[nstaged++].val = strdup(val); }
        } else if (!strcmp(cmd, "COMMIT")) {
            for (int i = 0; i < nstaged; i++) {
                int idx = find(committed, ncommitted, staged[i].key);
                if (idx != -1) { free(committed[idx].val); committed[idx].val = strdup(staged[i].val); }
                else { committed[ncommitted].key = strdup(staged[i].key); committed[ncommitted++].val = strdup(staged[i].val); }
            }
            for (int i = 0; i < nstaged; i++) { free(staged[i].key); free(staged[i].val); }
            nstaged = 0;
            in_transaction = 0;
        } else if (!strcmp(cmd, "ABORT")) {
            for (int i = 0; i < nstaged; i++) { free(staged[i].key); free(staged[i].val); }
            nstaged = 0;
            in_transaction = 0;
        } else if (!strcmp(cmd, "GET")) {
            scanf("%s", key);
            int idx = find(staged, nstaged, key);
            if (idx != -1) printf("%s\n", staged[idx].val);
            else {
                idx = find(committed, ncommitted, key);
                if (idx != -1) printf("%s\n", committed[idx].val);
            }
        } else if (!strcmp(cmd, "DUMP")) {
            for (int i = 0; i < ncommitted; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].val);
            }
        }
    }
    for (int i = 0; i < ncommitted; i++) { free(committed[i].key); free(committed[i].val); }
    for (int i = 0; i < nstaged; i++) { free(staged[i].key); free(staged[i].val); }
    return 0;
}