// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Config {
    char *key;
    char *value;
    struct Config *next;
} Config;

Config *configs = NULL;

Config *find_config(const char *key) {
    Config *curr = configs;
    while (curr) {
        if (strcmp(curr->key, key) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void set_config(const char *key, const char *val) {
    Config *c = find_config(key);
    if (c) {
        free(c->value);
        c->value = strdup(val);
    } else {
        c = malloc(sizeof(Config));
        if (!c) exit(EXIT_FAILURE);
        c->key = strdup(key);
        c->value = strdup(val);
        c->next = configs;
        configs = c;
    }
}

void unset_config(const char *key) {
    Config *curr = configs;
    Config *prev = NULL;
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

void get_config(const char *key) {
    Config *c = find_config(key);
    if (c) printf("%s\n", c->value);
}

void write_configs() {
    Config *curr = configs;
    while (curr) {
        printf("%s=%s\n", curr->key, curr->value);
        curr = curr->next;
    }
}

void free_all() {
    Config *curr = configs;
    while (curr) {
        Config *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            set_config(line, eq + 1);
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char key[256] = {0};
        
        sscanf(line, "%31s %255s", cmd, key);
        
        if (strcmp(cmd, "SET") == 0) {
            char *val_start = strchr(line, ' ');
            if (val_start) {
                val_start = strchr(val_start + 1, ' ');
                if (val_start) set_config(key, val_start + 1);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_config(key);
        } else if (strcmp(cmd, "GET") == 0) {
            get_config(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_configs();
        }
    }
    free_all();
    return 0;
}