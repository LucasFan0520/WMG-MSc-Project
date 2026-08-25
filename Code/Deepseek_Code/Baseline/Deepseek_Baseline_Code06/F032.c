/* F032.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Setting {
    char *key;
    char *value;
    struct Setting *next;
} Setting;

Setting *committed = NULL;
Setting *staged = NULL;
int in_transaction = 0;

Setting* find_setting(Setting *list, const char *key) {
    Setting *s = list;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void set_setting(Setting **list, const char *key, const char *value) {
    Setting *s = find_setting(*list, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        s->key = strdup(key);
        s->value = strdup(value);
        s->next = *list;
        *list = s;
    }
}

void unset_setting(Setting **list, const char *key) {
    Setting *s = *list, *prev = NULL;
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

void clear_list(Setting **list) {
    while (*list) {
        Setting *tmp = *list;
        *list = (*list)->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

void commit_transaction() {
    Setting *s = staged;
    while (s) {
        set_setting(&committed, s->key, s->value);
        s = s->next;
    }
    clear_list(&staged);
    in_transaction = 0;
}

void abort_transaction() {
    clear_list(&staged);
    in_transaction = 0;
}

void get_setting(const char *key) {
    Setting *s = NULL;
    if (in_transaction) s = find_setting(staged, key);
    if (!s) s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_settings() {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void free_all() {
    clear_list(&committed);
    clear_list(&staged);
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, key, value);
        if (strcmp(cmd, "SET") == 0 && n == 3) {
            set_setting(&committed, key, value);
        } else if (strcmp(cmd, "BEGIN") == 0 && n == 1) {
            if (!in_transaction) { in_transaction = 1; clear_list(&staged); }
        } else if (strcmp(cmd, "TSET") == 0 && n == 3) {
            if (in_transaction) set_setting(&staged, key, value);
        } else if (strcmp(cmd, "COMMIT") == 0 && n == 1) {
            if (in_transaction) commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0 && n == 1) {
            if (in_transaction) abort_transaction();
        } else if (strcmp(cmd, "GET") == 0 && n == 2) {
            get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0 && n == 1) {
            dump_settings();
        }
    }
    free_all();
    return 0;
}