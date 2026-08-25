/* F032.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct setting {
    char *key;
    char *value;
    struct setting *next;
} setting_t;

setting_t *committed = NULL;
setting_t *staged = NULL;
int in_transaction = 0;

setting_t *find_setting(setting_t *list, const char *key) {
    setting_t *s = list;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void set_setting(setting_t **list, const char *key, const char *value) {
    setting_t *s = find_setting(*list, key);
    if (s) {
        char *new = strdup(value);
        if (!new) return;
        free(s->value);
        s->value = new;
        return;
    }
    s = malloc(sizeof(setting_t));
    if (!s) return;
    s->key = strdup(key);
    s->value = strdup(value);
    s->next = *list;
    *list = s;
    if (!s->key || !s->value) {
        free(s->key); free(s->value); free(s);
    }
}

void unset_setting(setting_t **list, const char *key) {
    setting_t *prev = NULL, *s = *list;
    while (s) {
        if (strcmp(s->key, key) == 0) {
            if (prev) prev->next = s->next;
            else *list = s->next;
            free(s->key);
            free(s->value);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void free_list(setting_t *list) {
    setting_t *s = list;
    while (s) {
        setting_t *nxt = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = nxt;
    }
}

void begin_cmd(void) {
    if (in_transaction) return;
    in_transaction = 1;
    staged = NULL;
}

void tset_cmd(const char *key, const char *value) {
    if (!in_transaction) return;
    set_setting(&staged, key, value);
}

void commit_cmd(void) {
    if (!in_transaction) return;
    // apply staged to committed: for each staged, set in committed
    setting_t *s = staged;
    while (s) {
        set_setting(&committed, s->key, s->value);
        s = s->next;
    }
    free_list(staged);
    staged = NULL;
    in_transaction = 0;
}

void abort_cmd(void) {
    if (!in_transaction) return;
    free_list(staged);
    staged = NULL;
    in_transaction = 0;
}

void get_cmd(const char *key) {
    setting_t *s;
    if (in_transaction) {
        s = find_setting(staged, key);
        if (s) { printf("%s\n", s->value); return; }
    }
    s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
    else printf("NOT_FOUND\n");
}

void dump_cmd(void) {
    setting_t *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void free_all(void) {
    free_list(committed);
    free_list(staged);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], key[256];
        if (sscanf(line, "%9s %255s", cmd, key) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "SET") == 0) {
                const char *val = rest + strlen(key) + 1;
                set_setting(&committed, key, val);
            } else if (strcmp(cmd, "BEGIN") == 0) {
                begin_cmd();
            } else if (strcmp(cmd, "TSET") == 0) {
                const char *val = rest + strlen(key) + 1;
                tset_cmd(key, val);
            } else if (strcmp(cmd, "COMMIT") == 0) {
                commit_cmd();
            } else if (strcmp(cmd, "ABORT") == 0) {
                abort_cmd();
            } else if (strcmp(cmd, "GET") == 0) {
                get_cmd(key);
            } else if (strcmp(cmd, "DUMP") == 0) {
                dump_cmd();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}