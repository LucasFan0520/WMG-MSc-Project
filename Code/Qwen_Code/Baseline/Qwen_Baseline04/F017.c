// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} KV;

KV *kvs = NULL;
int kcount = 0;

int find_key(const char *key) {
    for (int i = 0; i < kcount; i++) {
        if (strcmp(kvs[i].key, key) == 0) return i;
    }
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1000];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = 0;
                    char *key = line;
                    char *val = eq + 1;
                    kvs = realloc(kvs, sizeof(KV) * (kcount + 1));
                    kvs[kcount].key = strdup(key);
                    kvs[kcount].val = strdup(val);
                    kcount++;
                }
            }
            fclose(f);
        }
    }
    
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "SET") == 0) {
            char key[100], val[1000];
            scanf("%s", key);
            scanf(" %[^\n]", val);
            int idx = find_key(key);
            if (idx != -1) {
                free(kvs[idx].val);
                kvs[idx].val = strdup(val);
            } else {
                kvs = realloc(kvs, sizeof(KV) * (kcount + 1));
                kvs[kcount].key = strdup(key);
                kvs[kcount].val = strdup(val);
                kcount++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[100];
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                free(kvs[idx].key);
                free(kvs[idx].val);
                for (int i = idx; i < kcount - 1; i++) kvs[i] = kvs[i+1];
                kcount--;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[100];
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                printf("%s\n", kvs[idx].val);
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < kcount; i++) {
                printf("%s=%s\n", kvs[i].key, kvs[i].val);
            }
        }
    }
    for (int i = 0; i < kcount; i++) {
        free(kvs[i].key);
        free(kvs[i].val);
    }
    free(kvs);
    return 0;
}