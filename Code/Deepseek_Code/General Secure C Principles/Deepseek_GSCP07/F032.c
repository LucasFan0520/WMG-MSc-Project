// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Setting {
    char *key;
    char *value;
    struct Setting *next;
} Setting;

Setting *committed = NULL;  // committed settings
Setting *staged = NULL;     // staged changes (only during transaction)

Setting* find_setting(Setting *list, const char *key) {
    Setting *s = list;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void free_setting_list(Setting *list) {
    while (list) {
        Setting *tmp = list;
        list = list->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

void handle_set(char *cmd) {
    char *key = strtok(NULL, " ");
    if (!key) return;
    char *val = strtok(NULL, "");
    if (!val) val = "";
    Setting *s = find_setting(committed, key);
    if (s) {
        free(s->value);
        s->value = strdup(val);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = strdup(key);
        new->value = strdup(val);
        new->next = NULL;
        if (!committed) committed = new;
        else {
            Setting *last = committed;
            while (last->next) last = last->next;
            last->next = new;
        }
    }
}

void handle_begin() {
    // If there is already a staged list, discard it? Spec says BEGIN starts transaction, probably no nested.
    // We'll clear any existing staged.
    free_setting_list(staged);
    staged = NULL;
}

void handle_tset(char *cmd) {
    if (!staged) {
        // If no transaction, ignore? Or maybe start implicitly? Spec says TSET stages a change, but must be inside transaction?
        // We'll just allow TSET only if staged list exists (transaction started).
        return;
    }
    char *key = strtok(NULL, " ");
    if (!key) return;
    char *val = strtok(NULL, "");
    if (!val) val = "";
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(val);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = strdup(key);
        new->value = strdup(val);
        new->next = NULL;
        if (!staged) staged = new;
        else {
            Setting *last = staged;
            while (last->next) last = last->next;
            last->next = new;
        }
    }
}

void handle_commit() {
    if (!staged) return;
    // Apply staged to committed
    Setting *s = staged;
    while (s) {
        // find in committed, update or add
        Setting *c = find_setting(committed, s->key);
        if (c) {
            free(c->value);
            c->value = strdup(s->value);
        } else {
            Setting *new = malloc(sizeof(Setting));
            new->key = strdup(s->key);
            new->value = strdup(s->value);
            new->next = NULL;
            if (!committed) committed = new;
            else {
                Setting *last = committed;
                while (last->next) last = last->next;
                last->next = new;
            }
        }
        s = s->next;
    }
    free_setting_list(staged);
    staged = NULL;
}

void handle_abort() {
    free_setting_list(staged);
    staged = NULL;
}

void handle_get(char *cmd) {
    char *key = strtok(NULL, " ");
    if (!key) return;
    Setting *s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void handle_dump() {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "SET") == 0) handle_set(cmd);
        else if (strcmp(cmd, "BEGIN") == 0) handle_begin();
        else if (strcmp(cmd, "TSET") == 0) handle_tset(cmd);
        else if (strcmp(cmd, "COMMIT") == 0) handle_commit();
        else if (strcmp(cmd, "ABORT") == 0) handle_abort();
        else if (strcmp(cmd, "GET") == 0) handle_get(cmd);
        else if (strcmp(cmd, "DUMP") == 0) handle_dump();
    }
    free_setting_list(committed);
    free_setting_list(staged);
    return 0;
}