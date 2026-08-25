/* F032.c */
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
int in_transaction = 0;

Setting *find_setting(Setting *list, const char *key) {
    while (list) {
        if (strcmp(list->key, key) == 0) return list;
        list = list->next;
    }
    return NULL;
}

void set_immediate(const char *key, const char *value) {
    Setting *s = find_setting(committed, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
        return;
    }
    Setting *new = malloc(sizeof(Setting));
    new->key = strdup(key);
    new->value = strdup(value);
    new->next = committed;
    committed = new;
}

void begin_transaction() {
    if (in_transaction) return;
    in_transaction = 1;
    staged = NULL;
}

void tset(const char *key, const char *value) {
    if (!in_transaction) return;
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
        return;
    }
    Setting *new = malloc(sizeof(Setting));
    new->key = strdup(key);
    new->value = strdup(value);
    new->next = staged;
    staged = new;
}

void commit() {
    if (!in_transaction) return;
    Setting *s = staged;
    while (s) {
        Setting *next = s->next;
        set_immediate(s->key, s->value);
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    staged = NULL;
    in_transaction = 0;
}

void abort_transaction() {
    if (!in_transaction) return;
    Setting *s = staged;
    while (s) { Setting *tmp = s; s = s->next; free(tmp->key); free(tmp->value); free(tmp); }
    staged = NULL;
    in_transaction = 0;
}

void get_value(const char *key) {
    Setting *s = find_setting(committed, key);
    if (!s) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", s->value);
}

void dump() {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], key[256], val[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "DUMP") == 0) { dump(); continue; }
        if (strcmp(cmd, "BEGIN") == 0) { begin_transaction(); continue; }
        if (strcmp(cmd, "COMMIT") == 0) { commit(); continue; }
        if (strcmp(cmd, "ABORT") == 0) { abort_transaction(); continue; }
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s", key) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(key) + 1;
            if (*rest == ' ') rest++;
            set_immediate(key, rest);
        } else if (strcmp(cmd, "TSET") == 0) {
            if (sscanf(line, "%*s %255s", key) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(key) + 1;
            if (*rest == ' ') rest++;
            tset(key, rest);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) get_value(key);
        }
    }
    return 0;
}