// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Config {
    char *key;
    char *value;
    struct Config *next;
} Config;

Config *configs = NULL;

Config *find_config(const char *key) {
    Config *c = configs;
    while (c) {
        if (strcmp(c->key, key) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void set_config(const char *key, const char *value) {
    Config *c = find_config(key);
    if (c) {
        free(c->value);
        c->value = strdup(value);
    } else {
        c = malloc(sizeof(Config));
        c->key = strdup(key);
        c->value = strdup(value);
        c->next = configs;
        configs = c;
    }
}

void unset_config(const char *key) {
    Config *c = find_config(key);
    if (!c) return;
    Config *cur = configs, *prev = NULL;
    while (cur) {
        if (cur == c) {
            if (prev) prev->next = cur->next;
            else configs = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    free(c->key);
    free(c->value);
    free(c);
}

void get_config(const char *key) {
    Config *c = find_config(key);
    if (c) printf("%s\n", c->value);
}

void write_configs(void) {
    Config *c = configs;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

void cleanup_configs(void) {
    Config *c = configs;
    while (c) {
        Config *tmp = c;
        c = c->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

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
                    char *key = line;
                    char *value = eq + 1;
                    set_config(key, value);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], key[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, key, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "SET") == 0 && n >= 3) {
            char *val = line + strlen(cmd) + 1 + strlen(key) + 1;
            set_config(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_configs();
        }
    }
    cleanup_configs();
    return 0;
}