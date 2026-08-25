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

Setting* find_setting(Setting *list, const char *key) {
    Setting *cur = list;
    while (cur) {
        if (strcmp(cur->key, key) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void set_committed(const char *key, const char *value) {
    Setting *s = find_setting(committed, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = NULL;
        if (!committed) committed = new;
        else {
            Setting *cur = committed;
            while (cur->next) cur = cur->next;
            cur->next = new;
        }
    }
}

void unset_committed(const char *key) {
    Setting *prev = NULL, *cur = committed;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else committed = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void tset(const char *key, const char *value) {
    if (!in_transaction) return;
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = NULL;
        if (!staged) staged = new;
        else {
            Setting *cur = staged;
            while (cur->next) cur = cur->next;
            cur->next = new;
        }
    }
}

void commit() {
    if (!in_transaction) return;
    // apply staged to committed
    Setting *cur = staged;
    while (cur) {
        set_committed(cur->key, cur->value);
        cur = cur->next;
    }
    // clear staged
    cur = staged;
    while (cur) {
        Setting *tmp = cur;
        cur = cur->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    staged = NULL;
    in_transaction = 0;
}

void abort_transaction() {
    if (!in_transaction) return;
    // discard staged
    Setting *cur = staged;
    while (cur) {
        Setting *tmp = cur;
        cur = cur->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    staged = NULL;
    in_transaction = 0;
}

void get_key(const char *key) {
    // check staged first? Spec says GET prints a value, likely from committed.
    // We'll return committed value.
    Setting *s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump() {
    Setting *cur = committed;
    while (cur) {
        printf("%s %s\n", cur->key, cur->value);
        cur = cur->next;
    }
}

void free_all() {
    Setting *cur = committed;
    while (cur) {
        Setting *tmp = cur;
        cur = cur->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    cur = staged;
    while (cur) {
        Setting *tmp = cur;
        cur = cur->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            set_committed(key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            if (!in_transaction) {
                in_transaction = 1;
                staged = NULL;
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            tset(key, val);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) get_key(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump();
        }
    }
    free_all();
    return 0;
}