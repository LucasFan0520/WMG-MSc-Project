// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct setting {
    char *key;
    char *value;
    struct setting *next;
};

struct setting *committed = NULL;
struct setting *staged = NULL;
int in_transaction = 0;

struct setting *find_setting(struct setting *list, char *key) {
    struct setting *s = list;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void set_setting(struct setting **list, char *key, char *value) {
    struct setting *s = find_setting(*list, key);
    if (s) {
        free(s->value);
        s->value = malloc(strlen(value) + 1);
        strcpy(s->value, value);
    } else {
        s = malloc(sizeof(struct setting));
        s->key = malloc(strlen(key) + 1);
        strcpy(s->key, key);
        s->value = malloc(strlen(value) + 1);
        strcpy(s->value, value);
        s->next = *list;
        *list = s;
    }
}

void unset_setting(struct setting **list, char *key) {
    struct setting *s = *list, *prev = NULL;
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

void get_setting(char *key) {
    struct setting *s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_settings() {
    struct setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void begin_transaction() {
    if (in_transaction) return;
    in_transaction = 1;
    staged = NULL;
}

void tset(char *key, char *value) {
    if (!in_transaction) return;
    set_setting(&staged, key, value);
}

void commit_transaction() {
    if (!in_transaction) return;
    // apply staged to committed
    struct setting *s = staged;
    while (s) {
        set_setting(&committed, s->key, s->value);
        s = s->next;
    }
    // clear staged
    while (staged) {
        struct setting *next = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = next;
    }
    in_transaction = 0;
}

void abort_transaction() {
    if (!in_transaction) return;
    while (staged) {
        struct setting *next = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = next;
    }
    in_transaction = 0;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, key, value);
        if (strcmp(cmd, "SET") == 0 && n >= 2) {
            set_setting(&committed, key, value);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0 && n >= 2) {
            tset(key, value);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0 && n >= 2) {
            get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    return 0;
}