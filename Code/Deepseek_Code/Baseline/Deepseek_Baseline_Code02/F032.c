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
        free(s->value);
        s->value = malloc(strlen(value) + 1);
        strcpy(s->value, value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = malloc(strlen(key) + 1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value) + 1);
        strcpy(new->value, value);
        new->next = committed;
        committed = new;
    }
}

void stage_setting(const char *key, const char *value) {
    if (!in_transaction) return;
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = malloc(strlen(value) + 1);
        strcpy(s->value, value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = malloc(strlen(key) + 1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value) + 1);
        strcpy(new->value, value);
        new->next = staged;
        staged = new;
    }
}

void begin_transaction(void) {
    if (in_transaction) return; // nested not supported
    in_transaction = 1;
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

void commit_transaction(void) {
    if (!in_transaction) return;
    // apply staged to committed
    Setting *s = staged;
    while (s) {
        set_committed(s->key, s->value);
        s = s->next;
    }
    // clear staged
    Setting *st = staged;
    while (st) {
        Setting *next = st->next;
        free(st->key);
        free(st->value);
        free(st);
        st = next;
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
    else printf("NOT_FOUND\n");
}

void dump_settings(void) {
    Setting *s = committed;
    while (s) {
        printf("%s=%s\n", s->key, s->value);
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            if (in_transaction) {
                // stage? The spec says SET changes immediately, even during transaction? It says "SET key value changes a setting immediately", so we ignore transaction.
            }
            set_committed(key, p);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            stage_setting(key, p);
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