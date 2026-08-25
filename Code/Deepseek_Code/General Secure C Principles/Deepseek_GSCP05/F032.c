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

Setting *find_setting(Setting *list, const char *key) {
    Setting *s = list;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void set_setting(const char *key, const char *value) {
    Setting *s = find_setting(committed, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        if (!new) return;
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = committed;
        committed = new;
    }
}

void begin_transaction(void) {
    // clear staged
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
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        if (!new) return;
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = staged;
        staged = new;
    }
}

void commit_transaction(void) {
    Setting *s = staged;
    while (s) {
        set_setting(s->key, s->value);
        s = s->next;
    }
    // clear staged
    Setting *cur = staged;
    while (cur) {
        Setting *next = cur->next;
        free(cur->key);
        free(cur->value);
        free(cur);
        cur = next;
    }
    staged = NULL;
}

void abort_transaction(void) {
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

void get_setting(const char *key) {
    Setting *s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_settings(void) {
    Setting *s = committed;
    while (s) {
        printf("%s: %s\n", s->key, s->value);
        s = s->next;
    }
}

void free_all(void) {
    Setting *s = committed;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            set_setting(key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            tset(key, val);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (space) *space = '\0';
            get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    free_all();
    return 0;
}