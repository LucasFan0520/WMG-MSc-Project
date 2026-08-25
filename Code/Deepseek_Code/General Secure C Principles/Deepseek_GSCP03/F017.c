/* F017.c */
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
        return;
    }
    Config *new_c = malloc(sizeof(Config));
    if (!new_c) return;
    new_c->key = strdup(key);
    new_c->value = strdup(value);
    new_c->next = head;
    head = new_c;
}

void unset_config(const char *key) {
    Config *prev = NULL, *curr = head;
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
        Config *tmp = c;
        c = c->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = line;
                    char *value = eq + 1;
                    set_config(key, value);
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key_end = p;
            while (*key_end && *key_end != ' ') key_end++;
            if (*key_end) {
                *key_end = '\0';
                char *val = key_end + 1;
                while (*val == ' ') val++;
                set_config(p, val);
            }
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