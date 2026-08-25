// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct conf {
    char *key;
    char *value;
    struct conf *next;
} Conf;

Conf *configs = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Conf *find_conf(const char *key) {
    Conf *c = configs;
    while (c) {
        if (strcmp(c->key, key) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void set_conf(const char *key, const char *value) {
    Conf *c = find_conf(key);
    if (c) {
        free(c->value);
        c->value = strdup(value);
        if (!c->value) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    Conf *newc = malloc(sizeof(Conf));
    if (!newc) { fprintf(stderr, "Memory error\n"); exit(1); }
    newc->key = strdup(key);
    newc->value = strdup(value);
    if (!newc->key || !newc->value) { fprintf(stderr, "Memory error\n"); exit(1); }
    newc->next = configs;
    configs = newc;
}

void unset_conf(const char *key) {
    Conf *c = configs, *prev = NULL;
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

void get_conf(const char *key) {
    Conf *c = find_conf(key);
    if (c) printf("%s\n", c->value);
}

void write_conf(void) {
    Conf *c = configs;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

void free_all(void) {
    Conf *c = configs;
    while (c) {
        Conf *next = c->next;
        free(c->key);
        free(c->value);
        free(c);
        c = next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        // trim spaces around key
        char *p = key + strlen(key) - 1;
        while (p > key && isspace(*p)) { *p = '\0'; p--; }
        // trim leading spaces in value? We'll keep as is.
        set_conf(key, value);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "WRITE") == 0) { write_conf(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "SET") == 0) {
            char *key = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            set_conf(key, rest);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_conf(rest);
        } else if (strcmp(cmd, "GET") == 0) {
            get_conf(rest);
        }
    }
    free_all();
    return 0;
}