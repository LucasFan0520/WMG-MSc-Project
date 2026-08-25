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

Config* find_config(const char *key) {
    Config *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void set_config(const char *key, const char *value) {
    Config *c = find_config(key);
    if (c) {
        free(c->value);
        c->value = strdup(value);
    } else {
        Config *new = malloc(sizeof(Config));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = NULL;
        if (!head) head = new;
        else {
            Config *cur = head;
            while (cur->next) cur = cur->next;
            cur->next = new;
        }
    }
}

void unset_config(const char *key) {
    Config *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
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
}

void write_all() {
    Config *cur = head;
    while (cur) {
        printf("%s=%s\n", cur->key, cur->value);
        cur = cur->next;
    }
}

void free_all() {
    Config *cur = head;
    while (cur) {
        Config *tmp = cur;
        cur = cur->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *key = strtok(line, "=");
                if (!key) continue;
                char *val = strtok(NULL, "");
                if (!val) val = "";
                set_config(key, val);
            }
            fclose(f);
        }
    }
    char cmdline[4096];
    while (fgets(cmdline, sizeof(cmdline), stdin)) {
        cmdline[strcspn(cmdline, "\n")] = 0;
        if (strcmp(cmdline, "END") == 0) break;
        char *cmd = strtok(cmdline, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            set_config(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_all();
        }
    }
    free_all();
    return 0;
}