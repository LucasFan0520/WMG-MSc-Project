// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } KV;
KV *config = NULL;
int conf_count = 0, conf_cap = 0;

int find_key(const char *key) {
    for (int i = 0; i < conf_count; i++)
        if (strcmp(config[i].key, key) == 0) return i;
    return -1;
}

void set_kv(const char *key, const char *val) {
    int idx = find_key(key);
    if (idx >= 0) {
        free(config[idx].val);
        config[idx].val = strdup(val);
    } else {
        if (conf_count == conf_cap) {
            conf_cap = conf_cap ? conf_cap * 2 : 4;
            config = realloc(config, sizeof(KV) * conf_cap);
        }
        config[conf_count].key = strdup(key);
        config[conf_count].val = strdup(val);
        conf_count++;
    }
}

void unset_key(const char *key) {
    int idx = find_key(key);
    if (idx < 0) return;
    free(config[idx].key);
    free(config[idx].val);
    for (int i = idx; i < conf_count - 1; i++) config[i] = config[i + 1];
    conf_count--;
}

void get_key(const char *key) {
    int idx = find_key(key);
    if (idx < 0) return;
    printf("%s\n", config[idx].val);
}

void write_config(void) {
    for (int i = 0; i < conf_count; i++)
        printf("%s=%s\n", config[i].key, config[i].val);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        set_kv(line, eq + 1);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], rest[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, rest) == 2)
                set_kv(arg1, rest);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) unset_key(arg1);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) get_key(arg1);
        } else if (strcmp(cmd, "WRITE") == 0) write_config();
    }
    for (int i = 0; i < conf_count; i++) { free(config[i].key); free(config[i].val); }
    free(config);
    return 0;
}