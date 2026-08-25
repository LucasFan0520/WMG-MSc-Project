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
        c = malloc(sizeof(Config));
        c->key = strdup(key);
        c->value = strdup(value);
        c->next = configs;
        configs = c;
    }
}

void unset_config(const char *key) {
    Config *c = configs, *prev = NULL;
    while (c && strcmp(c->key, key) != 0) {
        prev = c;
        c = c->next;
    }
    if (!c) return;
    if (prev) prev->next = c->next;
    else configs = c->next;
    free(c->key);
    free(c->value);
    free(c);
}

void get_config(const char *key) {
    Config *c = find_config(key);
    if (!c) return;
    printf("%s\n", c->value);
}

void write_config(void) {
    Config *c = configs;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

void free_all(void) {
    Config *c = configs;
    while (c) {
        Config *next = c->next;
        free(c->key);
        free(c->value);
        free(c);
        c = next;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char line[1024];
        while (fgets(line, sizeof(line), f)) {
            line[strcspn(line, "\n")] = '\0';
            char *eq = strchr(line, '=');
            if (!eq) continue;
            *eq = '\0';
            char *key = line;
            char *value = eq + 1;
            set_config(key, value);
        }
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p = '\0';
            p++;
            while (*p == ' ') p++;
            set_config(key, p);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            unset_config(p);
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            get_config(p);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_config();
        }
    }
    free_all();
    free(line);
    return 0;
}