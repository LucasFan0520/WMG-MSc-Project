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

Setting *find_setting(Setting *list, const char *key) {
    Setting *s = list;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void set_committed(const char *key, const char *value) {
    Setting *s = find_setting(committed, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
        return;
    }
    Setting *new_s = malloc(sizeof(Setting));
    if (!new_s) return;
    new_s->key = strdup(key);
    new_s->value = strdup(value);
    new_s->next = committed;
    committed = new_s;
}

void set_staged(const char *key, const char *value) {
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
        return;
    }
    Setting *new_s = malloc(sizeof(Setting));
    if (!new_s) return;
    new_s->key = strdup(key);
    new_s->value = strdup(value);
    new_s->next = staged;
    staged = new_s;
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
    set_staged(key, value);
}

void commit_transaction(void) {
    if (!in_transaction) return;
    // apply staged to committed
    Setting *s = staged;
    while (s) {
        set_committed(s->key, s->value);
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
    if (s) {
        printf("%s\n", s->value);
    }
}

void dump_settings(void) {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void free_all(void) {
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
                set_committed(p, val);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *key_end = p;
            while (*key_end && *key_end != ' ') key_end++;
            if (*key_end) {
                *key_end = '\0';
                char *val = key_end + 1;
                while (*val == ' ') val++;
                tset(p, val);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            get_setting(p);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    free_all();
    return 0;
}