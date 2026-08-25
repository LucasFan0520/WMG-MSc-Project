// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *val;
} Val;

typedef struct {
    char *key;
    Val *vals;
    int vc;
} Key;

Key keys[1000];
int kc = 0;

int find_key(char *key) {
    for (int i = 0; i < kc; i++) if (strcmp(keys[i].key, key) == 0) return i;
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
        
        if (strcmp(cmd, "ADD") == 0) {
            char key[1000], val[1000];
            sscanf(rest, "%s %s", key, val);
            int idx = find_key(key);
            if (idx == -1) {
                keys[kc].key = strdup(key);
                keys[kc].vals = NULL;
                keys[kc].vc = 0;
                idx = kc++;
            }
            keys[idx].vals = realloc(keys[idx].vals, sizeof(Val) * (keys[idx].vc + 1));
            keys[idx].vals[keys[idx].vc].val = strdup(val);
            keys[idx].vc++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[1000], val[1000];
            sscanf(rest, "%s %s", key, val);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < keys[idx].vc; i++) {
                    if (strcmp(keys[idx].vals[i].val, val) == 0) {
                        free(keys[idx].vals[i].val);
                        for (int j = i; j < keys[idx].vc - 1; j++) keys[idx].vals[j] = keys[idx].vals[j+1];
                        keys[idx].vc--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[1000];
            sscanf(rest, "%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                free(keys[idx].key);
                for (int i = 0; i < keys[idx].vc; i++) free(keys[idx].vals[i].val);
                free(keys[idx].vals);
                for (int i = idx; i < kc - 1; i++) keys[i] = keys[i+1];
                kc--;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[1000];
            sscanf(rest, "%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < keys[idx].vc; i++) printf("%s\n", keys[idx].vals[i].val);
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < kc; i++) printf("%s\n", keys[i].key);
        }
    }
    for (int i = 0; i < kc; i++) {
        free(keys[i].key);
        for (int j = 0; j < keys[i].vc; j++) free(keys[i].vals[j].val);
        free(keys[i].vals);
    }
    return 0;
}