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

Config *find_config(const char *key) {
    Config *c = head;
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
        c->value = malloc(strlen(value) + 1);
        strcpy(c->value, value);
        return;
    }
    c = malloc(sizeof(Config));
    c->key = malloc(strlen(key) + 1);
    strcpy(c->key, key);
    c->value = malloc(strlen(value) + 1);
    strcpy(c->value, value);
    c->next = head;
    head = c;
}

void unset_config(const char *key) {
    Config *prev = NULL;
    Config *c = head;
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
    Config *c = find_config(key);
    if (!c) return;
    printf("%s\n", c->value);
}

void write_config(void) {
    Config *c = head;
    while (c) {
        printf("%s=%s\n", c->key, c->value);
        c = c->next;
    }
}

void load_file(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = 0;
        char *key = line;
        char *value = eq + 1;
        if (strlen(key) > 0) {
            set_config(key, value);
        }
    }
    fclose(f);
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
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
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
    return 0;
}