// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Config {
    char *key;
    char *value;
    struct Config *next;
} Config;

Config *configs = NULL;

Config *find_config(const char *key) {
    for (Config *c = configs; c; c = c->next) {
        if (strcmp(c->key, key) == 0) return c;
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
    Config *prev = NULL, *curr = configs;
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

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line())) {
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            set_config(line, eq + 1);
        }
        free(line);
    }
    fclose(f);
}

void write_configs() {
    for (Config *c = configs; c; c = c->next) {
        printf("%s=%s\n", c->key, c->value);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, key[256] = {0};
        sscanf(line, "%15s %255s", cmd, key);
        char *value = strchr(line, ' ');
        if (value) {
            value++;
            while (*value == ' ') value++;
            value = strchr(value, ' ');
            if (value) {
                value++;
                while (*value == ' ') value++;
            } else {
                value = NULL;
            }
        }
        
        if (strcmp(cmd, "SET") == 0 && value) {
            set_config(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            Config *c = find_config(key);
            if (c) printf("%s\n", c->value);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_configs();
        }
        free(line);
    }
    
    Config *c = configs;
    while (c) {
        Config *next = c->next;
        free(c->key);
        free(c->value);
        free(c);
        c = next;
    }
    return 0;
}