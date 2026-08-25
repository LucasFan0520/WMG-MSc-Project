// F017.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Config;

static Config *configs = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_config(const char *key) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(configs[i].key, key) == 0)
            return (ssize_t)i;
    return -1;
}

static void set_config(const char *key, const char *value) {
    ssize_t idx = find_config(key);
    if (idx != -1) {
        free(configs[idx].value);
        configs[idx].value = strdup(value);
        if (!configs[idx].value) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Config *new = realloc(configs, capacity * sizeof(Config));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        configs = new;
    }
    configs[count].key = strdup(key);
    configs[count].value = strdup(value);
    if (!configs[count].key || !configs[count].value) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void unset_config(const char *key) {
    ssize_t idx = find_config(key);
    if (idx == -1) return;
    free(configs[idx].key);
    free(configs[idx].value);
    for (size_t i = idx; i < count - 1; i++)
        configs[i] = configs[i + 1];
    count--;
}

static void get_config(const char *key) {
    ssize_t idx = find_config(key);
    if (idx != -1)
        printf("%s\n", configs[idx].value);
}

static void write_configs(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s=%s\n", configs[i].key, configs[i].value);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(configs[i].key);
        free(configs[i].value);
    }
    free(configs);
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, f)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        set_config(line, eq + 1);
    }
    free(line);
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);

    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], key[256], val[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) == 2)
                set_config(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_configs();
        }
    }

    free(line);
    free_all();
    return 0;
}