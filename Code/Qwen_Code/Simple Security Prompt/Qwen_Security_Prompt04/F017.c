// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} KV;

KV kvs[10000];
int kc = 0;

int find_key(char *key) {
    for (int i = 0; i < kc; i++) if (strcmp(kvs[i].key, key) == 0) return i;
    return -1;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = 0;
            kvs[kc].key = strdup(line);
            kvs[kc].val = strdup(eq + 1);
            kc++;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
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
            int idx = find_key(key);
            if (idx == -1) {
                kvs[kc].key = strdup(key);
                kvs[kc].val = strdup(val);
                kc++;
            } else {
                free(kvs[idx].val);
                kvs[idx].val = strdup(val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[1000];
            sscanf(rest, "%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                free(kvs[idx].key); free(kvs[idx].val);
                for (int i = idx; i < kc - 1; i++) kvs[i] = kvs[i+1];
                kc--;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[1000];
            sscanf(rest, "%s", key);
            int idx = find_key(key);
            if (idx != -1) printf("%s\n", kvs[idx].val);
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < kc; i++) printf("%s=%s\n", kvs[i].key, kvs[i].val);
        }
    }
    for (int i = 0; i < kc; i++) {
        free(kvs[i].key); free(kvs[i].val);
    }
    return 0;
}