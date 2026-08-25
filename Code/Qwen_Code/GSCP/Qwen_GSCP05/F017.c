// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } KV;
KV *kvs = NULL; int kc = 0;

int find_kv(const char *k) {
    for (int i = 0; i < kc; i++) if (strcmp(kvs[i].key, k) == 0) return i;
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = 0;
                    int i = find_kv(line);
                    if (i >= 0) { free(kvs[i].val); kvs[i].val = strdup(eq + 1); }
                    else { kvs = realloc(kvs, (kc+1)*sizeof(KV)); kvs[kc].key = strdup(line); kvs[kc].val = strdup(eq + 1); kc++; }
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[700];
        c2[0] = c3[0] = 0;
        sscanf(line, "%19s %255s %700[^\n]", c1, c2, c3);
        
        if (strcmp(c1, "SET") == 0) {
            int i = find_kv(c2);
            if (i >= 0) { free(kvs[i].val); kvs[i].val = strdup(c3); }
            else { kvs = realloc(kvs, (kc+1)*sizeof(KV)); kvs[kc].key = strdup(c2); kvs[kc].val = strdup(c3); kc++; }
        } else if (strcmp(c1, "UNSET") == 0) {
            int i = find_kv(c2);
            if (i >= 0) { free(kvs[i].key); free(kvs[i].val); for(int j=i; j<kc-1; j++) kvs[j]=kvs[j+1]; kc--; }
        } else if (strcmp(c1, "GET") == 0) {
            int i = find_kv(c2);
            if (i >= 0) printf("%s\n", kvs[i].val);
        } else if (strcmp(c1, "WRITE") == 0) {
            for (int i = 0; i < kc; i++) printf("%s=%s\n", kvs[i].key, kvs[i].val);
        }
    }
    for (int i = 0; i < kc; i++) { free(kvs[i].key); free(kvs[i].val); }
    free(kvs);
    return 0;
}