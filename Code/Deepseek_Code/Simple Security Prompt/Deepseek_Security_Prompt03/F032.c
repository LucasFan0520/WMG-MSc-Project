/*
 * F032.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct setting {
    char *key;
    char *value;
    struct setting *next;
} Setting;

Setting *committed = NULL;
Setting *staged = NULL;

Setting *find_setting(Setting *list, const char *key) {
    while (list) {
        if (strcmp(list->key, key) == 0) return list;
        list = list->next;
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

void unset_committed(const char *key) {
    Setting *prev = NULL, *s = committed;
    while (s) {
        if (strcmp(s->key, key) == 0) {
            if (prev) prev->next = s->next;
            else committed = s->next;
            free(s->key);
            free(s->value);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void begin_transaction(void) {
    // discard any existing staged
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
}

void abort_transaction(void) {
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

void get_setting(const char *key) {
    Setting *s = find_setting(committed, key);
    if (!s) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", s->value);
}

void dump_settings(void) {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], key[256];
        if (sscanf(line, "%31s %255s", cmd, key) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(key) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            set_committed(key, rest);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            tset(key, rest);
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
    return 0;
}