// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } KV;
KV *kvs = 0; int n = 0, cap = 0;

int find(char *key) {
    for (int i=0; i<n; i++) if (!strcmp(kvs[i].key, key)) return i;
    return -1;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = 0;
        int i = find(line);
        if (i >= 0) { free(kvs[i].val); kvs[i].val = strdup(eq+1); }
        else {
            if (n == cap) { cap = cap ? cap*2 : 4; kvs = realloc(kvs, cap*sizeof(KV)); }
            kvs[n].key = strdup(line); kvs[n].val = strdup(eq+1); n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], key[256], val[700];
        val[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, key, val);
        if (!strcmp(cmd, "SET")) {
            int i = find(key);
            if (i >= 0) { free(kvs[i].val); kvs[i].val = strdup(val); }
            else {
                if (n == cap) { cap = cap ? cap*2 : 4; kvs = realloc(kvs, cap*sizeof(KV)); }
                kvs[n].key = strdup(key); kvs[n].val = strdup(val); n++;
            }
        } else if (!strcmp(cmd, "UNSET")) {
            int i = find(key);
            if (i >= 0) {
                free(kvs[i].key); free(kvs[i].val);
                for (int j=i; j<n-1; j++) kvs[j] = kvs[j+1]; n--;
            }
        } else if (!strcmp(cmd, "GET")) {
            int i = find(key);
            if (i >= 0) puts(kvs[i].val);
        } else if (!strcmp(cmd, "WRITE")) {
            for (int i=0; i<n; i++) printf("%s=%s\n", kvs[i].key, kvs[i].val);
        }
    }
    return 0;
}