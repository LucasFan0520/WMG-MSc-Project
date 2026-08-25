// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Config {
    char *key;
    char *value;
    struct Config *next;
} Config;

Config *configs = NULL;

Config* find_config(const char *key) {
    for (Config *c = configs; c; c = c->next)
        if (strcmp(c->key, key) == 0) return c;
    return NULL;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            Config *c = malloc(sizeof(Config));
            if (!c) continue;
            c->key = strdup(line);
            c->value = strdup(eq + 1);
            c->next = configs;
            configs = c;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", key, &n) != 1) continue;
            char *val = rest + n;
            while (*val == ' ') val++;
            Config *c = find_config(key);
            if (c) {
                free(c->value);
                c->value = strdup(val);
            } else {
                c = malloc(sizeof(Config));
                if (!c) continue;
                c->key = strdup(key);
                c->value = strdup(val);
                c->next = configs;
                configs = c;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) != 1) continue;
            Config *prev = NULL;
            for (Config *c = configs; c; prev = c, c = c->next) {
                if (strcmp(c->key, key) == 0) {
                    if (prev) prev->next = c->next;
                    else configs = c->next;
                    free(c->key);
                    free(c->value);
                    free(c);
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) != 1) continue;
            Config *c = find_config(key);
            if (c) printf("%s\n", c->value);
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (Config *c = configs; c; c = c->next)
                printf("%s=%s\n", c->key, c->value);
        }
    }
    while (configs) {
        Config *c = configs;
        configs = configs->next;
        free(c->key);
        free(c->value);
        free(c);
    }
    return 0;
}