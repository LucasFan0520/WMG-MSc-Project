// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Config {
    char* key;
    char* val;
    struct Config* next;
} Config;

Config* head = NULL;

Config* find_config(const char* key) {
    for (Config* c = head; c; c = c->next) {
        if (strcmp(c->key, key) == 0) return c;
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* f = fopen(argv[1], "r");
        if (f) {
            char l[MAX_LINE];
            while (fgets(l, sizeof(l), f)) {
                l[strcspn(l, "\n")] = 0;
                char* eq = strchr(l, '=');
                if (eq) {
                    *eq = '\0';
                    Config* n = malloc(sizeof(Config));
                    n->key = strdup(l);
                    n->val = strdup(eq + 1);
                    n->next = head;
                    head = n;
                }
            }
            fclose(f);
        }
    }
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "SET") == 0) {
            char* key = get_word(&p);
            Config* c = find_config(key);
            if (c) {
                free(c->val);
                c->val = strdup(p);
            } else {
                Config* n = malloc(sizeof(Config));
                n->key = strdup(key);
                n->val = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char* key = get_word(&p);
            Config* prev = NULL;
            for (Config* c = head; c; prev = c, c = c->next) {
                if (strcmp(c->key, key) == 0) {
                    if (prev) prev->next = c->next;
                    else head = c->next;
                    free(c->key);
                    free(c->val);
                    free(c);
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char* key = get_word(&p);
            Config* c = find_config(key);
            if (c) printf("%s\n", c->val);
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (Config* c = head; c; c = c->next) {
                printf("%s=%s\n", c->key, c->val);
            }
        }
    }
    return 0;
}