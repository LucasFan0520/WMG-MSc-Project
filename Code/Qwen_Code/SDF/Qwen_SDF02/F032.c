// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Setting {
    char *key;
    char *value;
    struct Setting *next;
} Setting;

Setting *committed = NULL;
Setting *staged = NULL;
int in_transaction = 0;

Setting* find_setting(Setting *list, const char *key) {
    for (Setting *s = list; s; s = s->next)
        if (strcmp(s->key, key) == 0) return s;
    return NULL;
}

void set_setting(Setting **list, const char *key, const char *value) {
    Setting *s = find_setting(*list, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        if (!s) return;
        s->key = strdup(key);
        s->value = strdup(value);
        if (!s->key || !s->value) { free(s->key); free(s->value); free(s); return; }
        s->next = *list;
        *list = s;
    }
}

void free_list(Setting **list) {
    while (*list) {
        Setting *s = *list;
        *list = s->next;
        free(s->key);
        free(s->value);
        free(s);
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", key, &n) != 1) continue;
            char *val = rest + n;
            while (*val == ' ') val++;
            set_setting(&committed, key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
            free_list(&staged);
        } else if (strcmp(cmd, "TSET") == 0) {
            if (!in_transaction) continue;
            char key[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", key, &n) != 1) continue;
            char *val = rest + n;
            while (*val == ' ') val++;
            set_setting(&staged, key, val);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (!in_transaction) continue;
            for (Setting *s = staged; s; s = s->next) {
                set_setting(&committed, s->key, s->value);
            }
            free_list(&staged);
            in_transaction = 0;
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (!in_transaction) continue;
            free_list(&staged);
            in_transaction = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) != 1) continue;
            Setting *s = find_setting(committed, key);
            if (s) printf("%s\n", s->value);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Setting *s = committed; s; s = s->next) {
                printf("%s=%s\n", s->key, s->value);
            }
        }
    }
    free_list(&committed);
    free_list(&staged);
    return 0;
}