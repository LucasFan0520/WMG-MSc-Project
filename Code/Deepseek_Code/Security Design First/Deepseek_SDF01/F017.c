// filename: F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Config {
    char *key;
    char *value;
    struct Config *next;
} Config;

static Config *configs = NULL;

static void cleanup(void) {
    while (configs) {
        Config *tmp = configs;
        configs = configs->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

static void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        // trim spaces? not required
        Config *c = malloc(sizeof(Config));
        if (!c) continue;
        c->key = strdup(key);
        c->value = strdup(value);
        if (!c->key || !c->value) {
            free(c->key);
            free(c->value);
            free(c);
            continue;
        }
        c->next = configs;
        configs = c;
    }
    free(line);
    fclose(f);
}

static Config *find_config(const char *key) {
    for (Config *c = configs; c; c = c->next)
        if (strcmp(c->key, key) == 0)
            return c;
    return NULL;
}

static void set_config(const char *key, const char *value) {
    Config *exist = find_config(key);
    if (exist) {
        char *newv = strdup(value);
        if (!newv) return;
        free(exist->value);
        exist->value = newv;
        return;
    }
    Config *c = malloc(sizeof(Config));
    if (!c) return;
    c->key = strdup(key);
    c->value = strdup(value);
    if (!c->key || !c->value) {
        free(c->key);
        free(c->value);
        free(c);
        return;
    }
    c->next = configs;
    configs = c;
}

static void unset_config(const char *key) {
    Config *prev = NULL;
    for (Config *c = configs; c; c = c->next) {
        if (strcmp(c->key, key) == 0) {
            if (prev) prev->next = c->next;
            else configs = c->next;
            free(c->key);
            free(c->value);
            free(c);
            return;
        }
        prev = c;
    }
}

static void get_config(const char *key) {
    Config *c = find_config(key);
    if (c) printf("%s\n", c->value);
}

static void write_configs(void) {
    for (Config *c = configs; c; c = c->next)
        printf("%s=%s\n", c->key, c->value);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "SET") == 0) {
            if (*arg1) set_config(arg1, arg2);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (*arg1) unset_config(arg1);
        } else if (strcmp(cmd, "GET") == 0) {
            if (*arg1) get_config(arg1);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_configs();
        }
    }
    free(line);
    return 0;
}