// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Config {
    char *key;
    char *value;
    struct Config *next;
} Config;

Config *head = NULL;

Config *find_key(const char *key) {
    Config *c = head;
    while (c) {
        if (strcmp(c->key, key) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void set_config(const char *key, const char *value) {
    Config *c = find_key(key);
    if (c) {
        char *newval = strdup(value);
        if (!newval) return;
        free(c->value);
        c->value = newval;
        return;
    }
    Config *nc = malloc(sizeof(Config));
    if (!nc) return;
    nc->key = strdup(key);
    nc->value = strdup(value);
    nc->next = NULL;
    if (!nc->key || !nc->value) {
        free(nc->key); free(nc->value); free(nc);
        return;
    }
    if (!head) head = nc;
    else {
        Config *last = head;
        while (last->next) last = last->next;
        last->next = nc;
    }
}

void unset_config(const char *key) {
    Config *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->key, key) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
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
    Config *c = find_key(key);
    if (c) printf("%s\n", c->value);
}

void write_config(void) {
    Config *c = head;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

void free_all(void) {
    Config *c = head;
    while (c) {
        Config *next = c->next;
        free(c->key);
        free(c->value);
        free(c);
        c = next;
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                line[strcspn(line, "\n")] = '\0';
                char key[256], value[4096];
                if (sscanf(line, "%255[^=]=%4095[^\n]", key, value) == 2) {
                    set_config(key, value);
                }
            }
            free(line);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], key[256], value[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, value) == 2)
                set_config(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_config();
        }
    }
    free(line);
    free_all();
    return 0;
}