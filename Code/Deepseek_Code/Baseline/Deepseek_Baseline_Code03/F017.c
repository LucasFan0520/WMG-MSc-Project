// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct config {
    char *key;
    char *value;
    struct config *next;
};

struct config *configs = NULL;

struct config *find_config(char *key) {
    struct config *c = configs;
    while (c) {
        if (strcmp(c->key, key) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void set_config(char *key, char *value) {
    struct config *c = find_config(key);
    if (c) {
        free(c->value);
        c->value = malloc(strlen(value) + 1);
        strcpy(c->value, value);
    } else {
        c = malloc(sizeof(struct config));
        c->key = malloc(strlen(key) + 1);
        strcpy(c->key, key);
        c->value = malloc(strlen(value) + 1);
        strcpy(c->value, value);
        c->next = configs;
        configs = c;
    }
}

void unset_config(char *key) {
    struct config *c = configs, *prev = NULL;
    while (c) {
        if (strcmp(c->key, key) == 0) {
            if (prev) prev->next = c->next;
            else configs = c->next;
            free(c->key);
            free(c->value);
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void get_config(char *key) {
    struct config *c = find_config(key);
    if (c) printf("%s\n", c->value);
}

void write_config() {
    struct config *c = configs;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[4096];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = 0;
            char key[256], value[4096];
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = 0;
                strcpy(key, line);
                strcpy(value, eq + 1);
                set_config(key, value);
            }
        }
        fclose(f);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256];
        char value[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, key, value);
        if (strcmp(cmd, "SET") == 0 && n >= 3) {
            set_config(key, value);
        } else if (strcmp(cmd, "UNSET") == 0 && n >= 2) {
            unset_config(key);
        } else if (strcmp(cmd, "GET") == 0 && n >= 2) {
            get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_config();
        }
    }
    return 0;
}