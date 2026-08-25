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

Setting *find_in_list(Setting *list, const char *key) {
    Setting *curr = list;
    while (curr) {
        if (strcmp(curr->key, key) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void set_in_list(Setting **list, const char *key, const char *value) {
    Setting *s = find_in_list(*list, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        if (!s) exit(EXIT_FAILURE);
        s->key = strdup(key);
        s->value = strdup(value);
        s->next = *list;
        *list = s;
    }
}

void set_setting(const char *key, const char *value) {
    if (in_transaction) {
        set_in_list(&staged, key, value);
    } else {
        set_in_list(&committed, key, value);
    }
}

void begin_transaction() {
    in_transaction = 1;
}

void commit_transaction() {
    Setting *curr = staged;
    while (curr) {
        set_in_list(&committed, curr->key, curr->value);
        curr = curr->next;
    }
    curr = staged;
    while (curr) {
        Setting *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
    staged = NULL;
    in_transaction = 0;
}

void abort_transaction() {
    Setting *curr = staged;
    while (curr) {
        Setting *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
    staged = NULL;
    in_transaction = 0;
}

void get_setting(const char *key) {
    Setting *s = find_in_list(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_settings() {
    Setting *curr = committed;
    while (curr) {
        printf("%s=%s\n", curr->key, curr->value);
        curr = curr->next;
    }
}

void free_all() {
    Setting *curr = committed;
    while (curr) {
        Setting *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
    curr = staged;
    while (curr) {
        Setting *next = curr->next;
        free(curr->key);
        free(curr->value);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char key[256] = {0};
        char value[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, key, value);
        
        if (strcmp(cmd, "SET") == 0 && matched >= 2) {
            set_setting(key, value);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0 && matched >= 2) {
            set_setting(key, value);
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
    free_all();
    return 0;
}