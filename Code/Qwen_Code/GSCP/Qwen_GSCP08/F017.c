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

Config *find_config(char *key) {
    for (Config *c = head; c; c = c->next) {
        if (strcmp(c->key, key) == 0) return c;
    }
    return NULL;
}

void set_config(char *key, char *value) {
    Config *c = find_config(key);
    if (!c) {
        c = malloc(sizeof(Config));
        c->key = strdup(key);
        c->next = head;
        head = c;
    } else {
        free(c->value);
    }
    c->value = strdup(value);
}

void unset_config(char *key) {
    Config *prev = NULL;
    Config *curr = head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void get_config(char *key) {
    Config *c = find_config(key);
    if (c) puts(c->value);
}

void write_config() {
    for (Config *c = head; c; c = c->next) {
        printf("%s=%s\n", c->key, c->value);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (file) {
            char line[1000];
            while (fgets(line, sizeof(line), file)) {
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = 0;
                    line[strcspn(line, "\n")] = 0;
                    set_config(line, eq + 1);
                }
            }
            fclose(file);
        }
    }
    char cmd[20], key[100], value[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%99s", key);
            fgets(value, sizeof(value), stdin);
            while (value[0] == ' ') memmove(value, value + 1, strlen(value));
            value[strcspn(value, "\n")] = 0;
            set_config(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%99s", key);
            unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%99s", key);
            get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_config();
        }
    }
    return 0;
}