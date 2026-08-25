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
    Config *prev = NULL;
    Config *c = configs;
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

void get_config(const char *key) {
    Config *c = find_config(key);
    if (!c) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = c->value; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void write_configs() {
    Config *c = configs;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

void free_all() {
    Config *c = configs;
    while (c) {
        Config *next = c->next;
        free(c->key);
        free(c->value);
        free(c);
        c = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
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
                    while (*value == ' ') value++;
                    set_config(key, value);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "SET") == 0) {
            char key[256], value[4096];
            if (sscanf(rest, "%255s %[^\n]", key, value) == 2) {
                set_config(key, value);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) {
                unset_config(key);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) {
                get_config(key);
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_configs();
        }
    }
    free_all();
    return 0;
}