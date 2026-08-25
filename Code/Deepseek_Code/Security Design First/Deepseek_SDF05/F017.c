// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void set_config(const char *key, const char *value) {
    Config *c = find_config(key);
    if (c) {
        free(c->value);
        c->value = safe_strdup(value);
    } else {
        c = malloc(sizeof(Config));
        if (!c) return;
        c->key = safe_strdup(key);
        c->value = safe_strdup(value);
        c->next = configs;
        configs = c;
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

void get_config(const char *key) {
    Config *c = find_config(key);
    if (c) printf("%s\n", c->value);
    else printf("NOT_FOUND\n");
}

void write_config(void) {
    Config *c = configs;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char key[512], value[MAX_LINE];
        if (sscanf(line, "%511[^=]=%[^\n]", key, value) == 2) {
            set_config(key, value);
        }
    }
    fclose(f);
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
    if (argc >= 2) load_file(argv[1]);

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "SET") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            set_config(arg1, p);
        } else if (strcmp(cmd, "UNSET") == 0 && n >= 2) {
            unset_config(arg1);
        } else if (strcmp(cmd, "GET") == 0 && n >= 2) {
            get_config(arg1);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_config();
        }
    }
    free_all();
    return 0;
}