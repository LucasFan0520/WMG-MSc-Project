// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Config {
    char *key;
    char *value;
    struct Config *next;
} Config;

Config *head = NULL, *tail = NULL;

Config *find_config(const char *key) {
    Config *c = head;
    while (c) {
        if (strcmp(c->key, key) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        char *key = line;
        char *value = eq + 1;
        if (*key && *value) {
            Config *c = find_config(key);
            if (c) {
                free(c->value);
                c->value = malloc(strlen(value) + 1);
                strcpy(c->value, value);
            } else {
                Config *new = malloc(sizeof(Config));
                new->key = malloc(strlen(key) + 1);
                strcpy(new->key, key);
                new->value = malloc(strlen(value) + 1);
                strcpy(new->value, value);
                new->next = NULL;
                if (tail) { tail->next = new; tail = new; }
                else head = tail = new;
            }
        }
    }
    free(line);
    fclose(f);
}

void set_config(const char *key, const char *value) {
    Config *c = find_config(key);
    if (c) {
        free(c->value);
        c->value = malloc(strlen(value) + 1);
        strcpy(c->value, value);
    } else {
        Config *new = malloc(sizeof(Config));
        new->key = malloc(strlen(key) + 1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value) + 1);
        strcpy(new->value, value);
        new->next = NULL;
        if (tail) { tail->next = new; tail = new; }
        else head = tail = new;
    }
}

void unset_config(const char *key) {
    Config *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->key);
            free(curr->value);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void get_config(const char *key) {
    Config *c = find_config(key);
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
    if (argc > 1) load_file(argv[1]);
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");
            if (key && value) set_config(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_config();
        }
    }
    free(line);
    free_all();
    return 0;
}