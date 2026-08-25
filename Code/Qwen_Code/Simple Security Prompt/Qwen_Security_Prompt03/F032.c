// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *value; } Setting;
Setting *committed = NULL;
int nc = 0;
Setting *staged = NULL;
int ns = 0;
int in_trans = 0;

int find(Setting *arr, int n, const char *key) {
    for (int i = 0; i < n; i++) if (strcmp(arr[i].key, key) == 0) return i;
    return -1;
}

void set_val(Setting **arr, int *n, const char *key, const char *val) {
    int idx = find(*arr, *n, key);
    if (idx != -1) {
        free((*arr)[idx].value);
        (*arr)[idx].value = strdup(val);
    } else {
        *arr = realloc(*arr, (*n + 1) * sizeof(Setting));
        (*arr)[*n].key = strdup(key);
        (*arr)[*n].value = strdup(val);
        (*n)++;
    }
}

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], k[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %s", k);
            char *v = strchr(line, ' '); v++; v = strchr(v, ' '); v++;
            if (in_trans) {
                set_val(&staged, &ns, k, v);
            } else {
                set_val(&committed, &nc, k, v);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
            for (int i = 0; i < ns; i++) { free(staged[i].key); free(staged[i].value); }
            free(staged);
            staged = NULL;
            ns = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            sscanf(line, "%*s %s", k);
            char *v = strchr(line, ' '); v++; v = strchr(v, ' '); v++;
            if (in_trans) set_val(&staged, &ns, k, v);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                for (int i = 0; i < ns; i++) {
                    set_val(&committed, &nc, staged[i].key, staged[i].value);
                }
                for (int i = 0; i < ns; i++) { free(staged[i].key); free(staged[i].value); }
                free(staged);
                staged = NULL;
                ns = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                for (int i = 0; i < ns; i++) { free(staged[i].key); free(staged[i].value); }
                free(staged);
                staged = NULL;
                ns = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            sscanf(line, "%*s %s", k);
            int idx = find(committed, nc, k);
            if (idx != -1) printf("%s\n", committed[idx].value);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < nc; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
    }
    for (int i = 0; i < nc; i++) { free(committed[i].key); free(committed[i].value); }
    free(committed);
    for (int i = 0; i < ns; i++) { free(staged[i].key); free(staged[i].value); }
    free(staged);
    return 0;
}