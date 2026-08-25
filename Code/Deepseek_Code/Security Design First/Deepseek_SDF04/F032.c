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

void set_committed(const char *key, const char *value) {
    Setting *s = find_setting(committed, key);
    if (s) {
        char *newval = strdup(value);
        if (!newval) return;
        free(s->value);
        s->value = newval;
    } else {
        Setting *ns = malloc(sizeof(Setting));
        if (!ns) return;
        ns->key = strdup(key);
        ns->value = strdup(value);
        ns->next = NULL;
        if (!ns->key || !ns->value) {
            free(ns->key); free(ns->value); free(ns);
            return;
        }
        if (!committed) committed = ns;
        else {
            Setting *last = committed;
            while (last->next) last = last->next;
            last->next = ns;
        }
    }
}

void begin_transaction(void) {
    if (in_transaction) return;
    in_transaction = 1;
    // clear any previous staged
    Setting *s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    staged = NULL;
}

void tset(const char *key, const char *value) {
    if (!in_transaction) return;
    Setting *s = find_setting(staged, key);
    if (s) {
        char *newval = strdup(value);
        if (!newval) return;
        free(s->value);
        s->value = newval;
    } else {
        Setting *ns = malloc(sizeof(Setting));
        if (!ns) return;
        ns->key = strdup(key);
        ns->value = strdup(value);
        ns->next = NULL;
        if (!ns->key || !ns->value) {
            free(ns->key); free(ns->value); free(ns);
            return;
        }
        if (!staged) staged = ns;
        else {
            Setting *last = staged;
            while (last->next) last = last->next;
            last->next = ns;
        }
    }
}

void commit_transaction(void) {
    if (!in_transaction) return;
    Setting *s = staged;
    while (s) {
        set_committed(s->key, s->value);
        s = s->next;
    }
    // clear staged
    s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    staged = NULL;
    in_transaction = 0;
}

void abort_transaction(void) {
    if (!in_transaction) return;
    Setting *s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
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

void free_settings(Setting *list) {
    Setting *s = list;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
}

void free_all(void) {
    free_settings(committed);
    free_settings(staged);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], key[256], value[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, value) == 2)
                set_committed(key, value);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, value) == 2)
                tset(key, value);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    free(line);
    free_all();
    return 0;
}