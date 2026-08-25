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
        c->value = strdup(value);
    } else {
        c = malloc(sizeof(Config));
        if (!c) return;
        c->key = strdup(key);
        c->value = strdup(value);
        c->next = head;
        head = c;
    }
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
    if (c) {
        for (char *p = c->value; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
    }
}

void write_config(void) {
    Config *c = head;
    while (c) {
        printf("%s=", c->key);
        for (char *p = c->value; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        c = c->next;
    }
}

void free_all(void) {
    while (head) {
        Config *next = head->next;
        free(head->key);
        free(head->value);
        free(head);
        head = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char key[256], value[1024];
                if (sscanf(line, "%255[^=]=%1023[^\n]", key, value) == 2) {
                    set_config(key, value);
                }
            }
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "SET") == 0) {
            char key[256], value[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", key, value) == 2) {
                set_config(key, value);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                unset_config(key);
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                get_config(key);
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_config();
        }
    }

    free(line);
    free_all();
    return 0;
}