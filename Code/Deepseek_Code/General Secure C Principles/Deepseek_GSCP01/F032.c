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
int transaction_active = 0;

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
    } else {
        s = malloc(sizeof(Setting));
        s->key = strdup(key);
        s->value = strdup(value);
        s->next = committed;
        committed = s;
    }
}

void get_setting(const char *key) {
    Setting *s = find_setting(committed, key);
    if (!s) return;
    printf("%s\n", s->value);
}

void dump_settings(void) {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void begin_transaction(void) {
    if (transaction_active) return;
    transaction_active = 1;
    staged = NULL;
}

void tset(const char *key, const char *value) {
    if (!transaction_active) return;
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        s->key = strdup(key);
        s->value = strdup(value);
        s->next = staged;
        staged = s;
    }
}

void commit_transaction(void) {
    if (!transaction_active) return;
    Setting *s = staged;
    while (s) {
        set_committed(s->key, s->value);
        s = s->next;
    }
    abort_transaction();
}

void abort_transaction(void) {
    if (!transaction_active) return;
    Setting *s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    staged = NULL;
    transaction_active = 0;
}

void free_list(Setting *list) {
    Setting *s = list;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p = '\0';
            p++;
            while (*p == ' ') p++;
            set_committed(key, p);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p = '\0';
            p++;
            while (*p == ' ') p++;
            tset(key, p);
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
    free_list(committed);
    free_list(staged);
    free(line);
    return 0;
}