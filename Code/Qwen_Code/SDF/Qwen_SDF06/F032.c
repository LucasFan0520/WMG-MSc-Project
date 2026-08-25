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

void set_setting(const char *key, const char *value) {
    Setting *target_list = in_transaction ? staged : committed;
    Setting *s = find_in(target_list, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        if (!s) return;
        s->key = strdup(key);
        s->value = strdup(value);
        if (!s->key || !s->value) { free(s->key); free(s->value); free(s); return; }
        s->next = target_list;
        if (in_transaction) staged = s;
        else committed = s;
    }
}

void begin_trans() {
    in_transaction = 1;
}

void commit_trans() {
    if (!in_transaction) return;
    for (Setting *s = staged; s; s = s->next) {
        Setting *c = find_in(committed, s->key);
        if (c) {
            free(c->value);
            c->value = strdup(s->value);
        } else {
            c = malloc(sizeof(Setting));
            if (!c) continue;
            c->key = strdup(s->key);
            c->value = strdup(s->value);
            if (!c->key || !c->value) { free(c->key); free(c->value); free(c); continue; }
            c->next = committed;
            committed = c;
        }
    }
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

void abort_trans() {
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
    Setting *s = find_in(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump() {
    for (Setting *s = committed; s; s = s->next) {
        printf("%s %s\n", s->key, s->value);
    }
}

void free_list(Setting *list) {
    while (list) {
        Setting *next = list->next;
        free(list->key);
        free(list->value);
        free(list);
        list = next;
    }
}

void free_all() {
    free_list(committed);
    free_list(staged);
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "SET") == 0 && n >= 3) {
            set_setting(arg1, arg2);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_trans();
        } else if (strcmp(cmd, "TSET") == 0 && n >= 3) {
            if (!in_transaction) begin_trans();
            set_setting(arg1, arg2);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_trans();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_trans();
        } else if (strcmp(cmd, "GET") == 0 && n >= 2) {
            get_setting(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump();
        }
    }
    free_all();
    return 0;
}