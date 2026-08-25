// F032.c
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

Setting *find_setting(Setting *list, const char *key) {
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
    Setting *cur = *list, *prev = NULL;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else *list = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void begin_transaction(void) {
    if (in_transaction) return;
    in_transaction = 1;
    // clear staged
    Setting *s = staged;
    while (s) {
        Setting *tmp = s;
        s = s->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    staged = NULL;
}

void tset(const char *key, const char *value) {
    if (!in_transaction) return;
    set_setting(&staged, key, value);
}

void commit_transaction(void) {
    if (!in_transaction) return;
    // apply staged to committed
    Setting *s = staged;
    while (s) {
        set_setting(&committed, s->key, s->value);
        s = s->next;
    }
    // clear staged
    s = staged;
    while (s) {
        Setting *tmp = s;
        s = s->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    staged = NULL;
    in_transaction = 0;
}

void abort_transaction(void) {
    if (!in_transaction) return;
    Setting *s = staged;
    while (s) {
        Setting *tmp = s;
        s = s->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    staged = NULL;
    in_transaction = 0;
}

void get_setting(const char *key) {
    Setting *s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_settings(void) {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void cleanup_settings(void) {
    Setting *s = committed;
    while (s) {
        Setting *tmp = s;
        s = s->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    s = staged;
    while (s) {
        Setting *tmp = s;
        s = s->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], key[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, key, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "SET") == 0 && n >= 3) {
            char *val = line + strlen(cmd) + 1 + strlen(key) + 1;
            set_setting(&committed, key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0 && n >= 3) {
            char *val = line + strlen(cmd) + 1 + strlen(key) + 1;
            tset(key, val);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    cleanup_settings();
    return 0;
}