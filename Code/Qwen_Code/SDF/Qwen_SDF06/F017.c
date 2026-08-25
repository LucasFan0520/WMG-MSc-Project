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

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char key[1024], value[4096];
        if (sscanf(line, "%1023[^=]=%4095[^\n]", key, value) == 2) {
            Config *c = malloc(sizeof(Config));
            if (!c) continue;
            c->key = strdup(key);
            c->value = strdup(value);
            c->next = head;
            head = c;
        }
    }
    fclose(f);
}

void set_config(const char *key, const char *value) {
    for (Config *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->key, key) == 0) {
            free(curr->value);
            curr->value = strdup(value);
            return;
        }
    }
    Config *c = malloc(sizeof(Config));
    if (!c) return;
    c->key = strdup(key);
    c->value = strdup(value);
    c->next = head;
    head = c;
}

void unset_config(const char *key) {
    Config *curr = head;
    Config *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            Config *next = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void get_config(const char *key) {
    for (Config *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->key, key) == 0) {
            printf("%s\n", curr->value);
            return;
        }
    }
}

void write_config() {
    for (Config *curr = head; curr; curr = curr->next) {
        printf("%s=%s\n", curr->key, curr->value);
    }
}

void free_all() {
    Config *curr = head;
    while (curr) {
        Config *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char key[1024], value[4096];
        
        int n = sscanf(line, "%15s %1023s %4095[^\n]", cmd, key, value);
        if (strcmp(cmd, "SET") == 0 && n == 3) {
            set_config(key, value);
        } else if (strcmp(cmd, "UNSET") == 0 && n >= 2) {
            unset_config(key);
        } else if (strcmp(cmd, "GET") == 0 && n >= 2) {
            get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_config();
        }
    }
    free_all();
    return 0;
}