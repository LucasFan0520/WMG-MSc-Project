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

Setting *find_in(Setting *list, const char *key) {
    for (Setting *s = list; s; s = s->next) {
        if (strcmp(s->key, key) == 0) return s;
    }
    return NULL;
}

void set_in(Setting **list, const char *key, const char *value) {
    Setting *s = find_in(*list, key);
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

void set_setting(const char *key, const char *value) {
    set_in(&committed, key, value);
}

void tset_setting(const char *key, const char *value) {
    if (in_transaction) {
        set_in(&staged, key, value);
    }
}

void begin_trans() {
    in_transaction = 1;
}

void commit_trans() {
    while (staged) {
        Setting *tmp = staged->next;
        set_in(&committed, staged->key, staged->value);
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = tmp;
    }
    in_transaction = 0;
}

void abort_trans() {
    while (staged) {
        Setting *tmp = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = tmp;
    }
    in_transaction = 0;
}

void get_setting(const char *key) {
    Setting *s = find_in(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_settings() {
    for (Setting *s = committed; s; s = s->next) {
        printf("%s %s\n", s->key, s->value);
    }
}

void cleanup() {
    while (committed) {
        Setting *tmp = committed->next;
        free(committed->key);
        free(committed->value);
        free(committed);
        committed = tmp;
    }
    while (staged) {
        Setting *tmp = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) set_setting(a1, t + 1); }
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) tset_setting(a1, t + 1); }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_trans();
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_trans();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_trans();
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) get_setting(a1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    cleanup();
    return 0;
}