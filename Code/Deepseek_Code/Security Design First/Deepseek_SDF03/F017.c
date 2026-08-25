// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_KEY 64
#define MAX_VAL 4096

typedef struct Config {
    char key[MAX_KEY];
    char val[MAX_VAL];
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

void free_all() {
    Config *c = head;
    while (c) {
        Config *tmp = c;
        c = c->next;
        free(tmp);
    }
    head = NULL;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char key[MAX_KEY], val[MAX_VAL];
        if (sscanf(line, "%63[^=]=%4095[^\n]", key, val) == 2) {
            if (find_config(key)) continue;
            Config *c = malloc(sizeof(Config));
            if (!c) continue;
            strcpy(c->key, key);
            strcpy(c->val, val);
            c->next = head;
            head = c;
        }
    }
    fclose(f);
}

void handle_set(char *line) {
    char key[MAX_KEY], val[MAX_VAL];
    int n = sscanf(line, "SET %63s %4095[^\n]", key, val);
    if (n < 2) return;
    Config *c = find_config(key);
    if (c) {
        strcpy(c->val, val);
    } else {
        Config *newc = malloc(sizeof(Config));
        if (!newc) return;
        strcpy(newc->key, key);
        strcpy(newc->val, val);
        newc->next = head;
        head = newc;
    }
}

void handle_unset(char *line) {
    char key[MAX_KEY];
    if (sscanf(line, "UNSET %63s", key) != 1) return;
    Config *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->key, key) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void handle_get(char *line) {
    char key[MAX_KEY];
    if (sscanf(line, "GET %63s", key) != 1) return;
    Config *c = find_config(key);
    if (c) printf("%s\n", c->val);
}

void handle_write() {
    Config *c = head;
    while (c) {
        printf("%s=%s\n", c->key, c->val);
        c = c->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "SET ", 4) == 0) handle_set(line);
        else if (strncmp(line, "UNSET ", 6) == 0) handle_unset(line);
        else if (strncmp(line, "GET ", 4) == 0) handle_get(line);
        else if (strcmp(line, "WRITE") == 0) handle_write();
    }
    free_all();
    return 0;
}