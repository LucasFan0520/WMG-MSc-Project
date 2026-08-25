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

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = 0;
            Config *c = malloc(sizeof(Config));
            c->key = strdup(line);
            c->value = strdup(eq + 1);
            c->next = configs;
            configs = c;
        }
    }
    fclose(f);
}

Config *find_config(const char *key) {
    Config *curr = configs;
    while (curr) {
        if (strcmp(curr->key, key) == 0) return curr;
        curr = curr->next;
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
    Config *curr = configs, *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else configs = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void get_config(const char *key) {
    Config *c = find_config(key);
    if (c) printf("%s\n", c->value);
}

void write_configs() {
    Config *curr = configs;
    while (curr) {
        printf("%s=%s\n", curr->key, curr->value);
        curr = curr->next;
    }
}

void free_all() {
    Config *curr = configs;
    while (curr) {
        Config *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], key[256], value[4096] = {0};
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, key, value);
        
        if (strcmp(cmd, "SET") == 0 && n >= 2) set_config(key, value);
        else if (strcmp(cmd, "UNSET") == 0 && n >= 1) unset_config(key);
        else if (strcmp(cmd, "GET") == 0 && n >= 1) get_config(key);
        else if (strcmp(cmd, "WRITE") == 0) write_configs();
    }
    free_all();
    return 0;
}