// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Setting;

Setting committed[1000];
int cc = 0;

Setting staged[1000];
int sc = 0;

int in_transaction = 0;

int find_key(Setting *arr, int count, char *key) {
    for (int i = 0; i < count; i++) if (strcmp(arr[i].key, key) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "SET") == 0) {
            char key[1000];
            sscanf(rest, "%s", key);
            char *val = rest + strlen(key);
            while (*val == ' ') val++;
            if (in_transaction) {
                int idx = find_key(staged, sc, key);
                if (idx == -1) {
                    staged[sc].key = strdup(key);
                    staged[sc].val = strdup(val);
                    sc++;
                } else {
                    free(staged[idx].val);
                    staged[idx].val = strdup(val);
                }
            } else {
                int idx = find_key(committed, cc, key);
                if (idx == -1) {
                    committed[cc].key = strdup(key);
                    committed[cc].val = strdup(val);
                    cc++;
                } else {
                    free(committed[idx].val);
                    committed[idx].val = strdup(val);
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
            for (int i = 0; i < sc; i++) {
                free(staged[i].key);
                free(staged[i].val);
            }
            sc = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            char key[1000];
            sscanf(rest, "%s", key);
            char *val = rest + strlen(key);
            while (*val == ' ') val++;
            int idx = find_key(staged, sc, key);
            if (idx == -1) {
                staged[sc].key = strdup(key);
                staged[sc].val = strdup(val);
                sc++;
            } else {
                free(staged[idx].val);
                staged[idx].val = strdup(val);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            for (int i = 0; i < sc; i++) {
                int idx = find_key(committed, cc, staged[i].key);
                if (idx == -1) {
                    committed[cc].key = strdup(staged[i].key);
                    committed[cc].val = strdup(staged[i].val);
                    cc++;
                } else {
                    free(committed[idx].val);
                    committed[idx].val = strdup(staged[i].val);
                }
                free(staged[i].key);
                free(staged[i].val);
            }
            sc = 0;
            in_transaction = 0;
        } else if (strcmp(cmd, "ABORT") == 0) {
            for (int i = 0; i < sc; i++) {
                free(staged[i].key);
                free(staged[i].val);
            }
            sc = 0;
            in_transaction = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            int idx = find_key(committed, cc, rest);
            if (idx != -1) printf("%s\n", committed[idx].val);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < cc; i++) {
                printf("%s %s\n", committed[i].key, committed[i].val);
            }
        }
    }
    for (int i = 0; i < cc; i++) {
        free(committed[i].key);
        free(committed[i].val);
    }
    for (int i = 0; i < sc; i++) {
        free(staged[i].key);
        free(staged[i].val);
    }
    return 0;
}