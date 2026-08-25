// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } KV;
KV *kv = NULL;
int n = 0;

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = 0;
                    kv = realloc(kv, (n + 1) * sizeof(KV));
                    kv[n].key = strdup(line); kv[n].val = strdup(eq + 1); n++;
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], k[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %s", k);
            char *v = strchr(line, ' '); v++; v = strchr(v, ' ');
            int idx = -1;
            for (int i = 0; i < n; i++) if (strcmp(kv[i].key, k) == 0) { idx = i; break; }
            if (idx != -1) { free(kv[idx].val); kv[idx].val = strdup(v ? v + 1 : ""); }
            else { kv = realloc(kv, (n + 1) * sizeof(KV)); kv[n].key = strdup(k); kv[n].val = strdup(v ? v + 1 : ""); n++; }
        } else if (strcmp(cmd, "UNSET") == 0) {
            sscanf(line, "%*s %s", k);
            for (int i = 0; i < n; i++) if (strcmp(kv[i].key, k) == 0) {
                free(kv[i].key); free(kv[i].val);
                for (int j = i; j < n - 1; j++) kv[j] = kv[j + 1];
                n--; break;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            sscanf(line, "%*s %s", k);
            for (int i = 0; i < n; i++) if (strcmp(kv[i].key, k) == 0) { printf("%s\n", kv[i].val); break; }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < n; i++) printf("%s=%s\n", kv[i].key, kv[i].val);
        }
    }
    for (int i = 0; i < n; i++) { free(kv[i].key); free(kv[i].val); }
    free(kv);
    return 0;
}