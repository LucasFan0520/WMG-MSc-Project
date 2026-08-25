// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} KV;

KV *kvs = NULL;
int n = 0;

int find_kv(const char *key) {
    for (int i = 0; i < n; i++)
        if (strcmp(kvs[i].key, key) == 0) return i;
    return -1;
}

void load(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = 0;
            char *val = eq + 1;
            if (*val == ' ') val++;
            kvs = realloc(kvs, sizeof(KV) * (n + 1));
            kvs[n].key = strdup(line);
            kvs[n].val = strdup(val);
            n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *p = line + 4 + strlen(arg1);
            if (*p == ' ') p++;
            int i = find_kv(arg1);
            if (i != -1) {
                free(kvs[i].val);
                kvs[i].val = strdup(p);
            } else {
                kvs = realloc(kvs, sizeof(KV) * (n + 1));
                kvs[n].key = strdup(arg1);
                kvs[n].val = strdup(p);
                n++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_kv(arg1);
            if (i != -1) {
                free(kvs[i].key);
                free(kvs[i].val);
                for (int j = i; j < n - 1; j++) kvs[j] = kvs[j + 1];
                n--;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_kv(arg1);
            if (i != -1) printf("%s\n", kvs[i].val);
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < n; i++) printf("%s=%s\n", kvs[i].key, kvs[i].val);
        }
    }
    for (int i = 0; i < n; i++) {
        free(kvs[i].key);
        free(kvs[i].val);
    }
    free(kvs);
    return 0;
}