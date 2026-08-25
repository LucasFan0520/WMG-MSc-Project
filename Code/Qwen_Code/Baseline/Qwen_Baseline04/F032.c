// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Setting;

Setting *committed = NULL;
int ccount = 0;

Setting *staged = NULL;
int scount = 0;

int in_transaction = 0;

int find(Setting *arr, int count, const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(arr[i].key, key) == 0) return i;
    }
    return -1;
}

void set_val(Setting **arr, int *count, const char *key, const char *val) {
    int idx = find(*arr, *count, key);
    if (idx != -1) {
        free((*arr)[idx].val);
        (*arr)[idx].val = strdup(val);
    } else {
        *arr = realloc(*arr, sizeof(Setting) * (*count + 1));
        (*arr)[*count].key = strdup(key);
        (*arr)[*count].val = strdup(val);
        (*count)++;
    }
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "SET") == 0) {
            char key[100], val[1000];
            scanf("%s", key);
            scanf(" %[^\n]", val);
            set_val(&committed, &ccount, key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
            for (int i = 0; i < scount; i++) {
                free(staged[i].key);
                free(staged[i].val);
            }
            free(staged);
            staged = NULL;
            scount = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            char key[100], val[1000];
            scanf("%s", key);
            scanf(" %[^\n]", val);
            if (in_transaction) {
                set_val(&staged, &scount, key, val);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_transaction) {
                for (int i = 0; i < scount; i++) {
                    set_val(&committed, &ccount, staged[i].key, staged[i].val);
                }
                for (int i = 0; i < scount; i++) {
                    free(staged[i].key);
                    free(staged[i].val);
                }
                free(staged);
                staged = NULL;
                scount = 0;
                in_transaction = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_transaction) {
                for (int i = 0; i < scount; i++) {
                    free(staged[i].key);
                    free(staged[i].val);
                }
                free(staged);
                staged = NULL;
                scount = 0;
                in_transaction = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[100];
            scanf("%s", key);
            int idx = find(committed, ccount, key);
            if (idx != -1) {
                printf("%s\n", committed[idx].val);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s %s\n", committed[i].key, committed[i].val);
            }
        }
    }
    for (int i = 0; i < ccount; i++) {
        free(committed[i].key);
        free(committed[i].val);
    }
    free(committed);
    for (int i = 0; i < scount; i++) {
        free(staged[i].key);
        free(staged[i].val);
    }
    free(staged);
    return 0;
}