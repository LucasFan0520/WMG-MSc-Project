// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } KV;
KV kv[10000]; int nk = 0;

int find(char *key) {
    for (int i = 0; i < nk; i++) if (!strcmp(kv[i].key, key)) return i;
    return -1;
}

void load(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = 0;
            kv[nk].key = strdup(line); kv[nk++].val = strdup(eq + 1);
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char cmd[20], key[100], val[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "SET")) {
            scanf("%s", key); fgets(val, sizeof(val), stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            int idx = find(key);
            if (idx != -1) { free(kv[idx].val); kv[idx].val = strdup(val); }
            else { kv[nk].key = strdup(key); kv[nk++].val = strdup(val); }
        } else if (!strcmp(cmd, "UNSET")) {
            scanf("%s", key); int idx = find(key);
            if (idx != -1) {
                free(kv[idx].key); free(kv[idx].val);
                for (int i = idx; i < nk - 1; i++) kv[i] = kv[i + 1];
                nk--;
            }
        } else if (!strcmp(cmd, "GET")) {
            scanf("%s", key); int idx = find(key);
            if (idx != -1) printf("%s\n", kv[idx].val);
        } else if (!strcmp(cmd, "WRITE")) {
            for (int i = 0; i < nk; i++) printf("%s=%s\n", kv[i].key, kv[i].val);
        }
    }
    for (int i = 0; i < nk; i++) { free(kv[i].key); free(kv[i].val); }
    return 0;
}