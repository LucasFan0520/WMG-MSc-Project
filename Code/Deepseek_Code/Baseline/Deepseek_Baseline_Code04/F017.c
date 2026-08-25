/* F017.c */
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
        Config *nc = malloc(sizeof(Config));
        nc->key = strdup(key);
        nc->value = strdup(value);
        nc->next = configs;
        configs = nc;
    }
}

void unset_config(const char *key) {
    Config *prev = NULL, *cur = configs;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else configs = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void write_configs(void) {
    Config *c = configs;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

void free_configs(void) {
    while (configs) {
        Config *tmp = configs;
        configs = configs->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[4096];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = '\0';
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *key = line;
                char *value = eq + 1;
                set_config(key, value);
            }
        }
        fclose(f);
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char key[256];
        int n = sscanf(line, "%s %s", cmd, key);
        if (strcmp(cmd, "SET") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(key) + 1;
            while (*p == ' ') p++;
            if (*p) set_config(key, p);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (n < 2) continue;
            unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            if (n < 2) continue;
            Config *c = find_config(key);
            if (c) printf("%s\n", c->value);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_configs();
        }
    }
    free_configs();
    return 0;
}