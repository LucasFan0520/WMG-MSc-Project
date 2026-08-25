// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_KEY 64
#define MAX_VAL 4096

typedef struct Setting {
    char key[MAX_KEY];
    char val[MAX_VAL];
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

void free_settings(Setting *list) {
    Setting *s = list;
    while (s) {
        Setting *tmp = s;
        s = s->next;
        free(tmp);
    }
}

void handle_set(char *line) {
    char key[MAX_KEY], val[MAX_VAL];
    int n = sscanf(line, "SET %63s %4095[^\n]", key, val);
    if (n < 2) return;
    Setting *s = find_setting(committed, key);
    if (s) {
        strcpy(s->val, val);
    } else {
        Setting *new = malloc(sizeof(Setting));
        if (!new) return;
        strcpy(new->key, key);
        strcpy(new->val, val);
        new->next = committed;
        committed = new;
    }
}

void handle_begin() {
    if (in_transaction) return;
    in_transaction = 1;
    // staged starts empty
    staged = NULL;
}

void handle_tset(char *line) {
    if (!in_transaction) return;
    char key[MAX_KEY], val[MAX_VAL];
    int n = sscanf(line, "TSET %63s %4095[^\n]", key, val);
    if (n < 2) return;
    Setting *s = find_setting(staged, key);
    if (s) {
        strcpy(s->val, val);
    } else {
        Setting *new = malloc(sizeof(Setting));
        if (!new) return;
        strcpy(new->key, key);
        strcpy(new->val, val);
        new->next = staged;
        staged = new;
    }
}

void handle_commit() {
    if (!in_transaction) return;
    // apply staged to committed
    Setting *s = staged;
    while (s) {
        Setting *existing = find_setting(committed, s->key);
        if (existing) {
            strcpy(existing->val, s->val);
        } else {
            Setting *new = malloc(sizeof(Setting));
            if (!new) { free_settings(staged); staged = NULL; in_transaction = 0; return; }
            strcpy(new->key, s->key);
            strcpy(new->val, s->val);
            new->next = committed;
            committed = new;
        }
        s = s->next;
    }
    free_settings(staged);
    staged = NULL;
    in_transaction = 0;
}

void handle_abort() {
    if (!in_transaction) return;
    free_settings(staged);
    staged = NULL;
    in_transaction = 0;
}

void handle_get(char *line) {
    char key[MAX_KEY];
    if (sscanf(line, "GET %63s", key) != 1) return;
    Setting *s = find_setting(committed, key);
    if (s) printf("%s\n", s->val);
}

void handle_dump() {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->val);
        s = s->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "SET ", 4) == 0) handle_set(line);
        else if (strcmp(line, "BEGIN") == 0) handle_begin();
        else if (strncmp(line, "TSET ", 5) == 0) handle_tset(line);
        else if (strcmp(line, "COMMIT") == 0) handle_commit();
        else if (strcmp(line, "ABORT") == 0) handle_abort();
        else if (strncmp(line, "GET ", 4) == 0) handle_get(line);
        else if (strcmp(line, "DUMP") == 0) handle_dump();
    }
    free_settings(committed);
    free_settings(staged);
    return 0;
}