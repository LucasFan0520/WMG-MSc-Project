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

Setting *find_setting(Setting *list, char *key) {
    for (Setting *s = list; s; s = s->next) {
        if (strcmp(s->key, key) == 0) return s;
    }
    return NULL;
}

void set_setting(Setting **list, char *key, char *value) {
    Setting *s = find_setting(*list, key);
    if (!s) {
        s = malloc(sizeof(Setting));
        s->key = strdup(key);
        s->next = *list;
        *list = s;
    } else {
        free(s->value);
    }
    s->value = strdup(value);
}

void unset_setting(Setting **list, char *key) {
    Setting *prev = NULL;
    Setting *curr = *list;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else *list = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void commit_transaction() {
    for (Setting *s = staged; s; s = s->next) {
        set_setting(&committed, s->key, s->value);
    }
    while (staged) {
        Setting *next = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = next;
    }
    in_transaction = 0;
}

void abort_transaction() {
    while (staged) {
        Setting *next = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = next;
    }
    in_transaction = 0;
}

void get_setting(char *key) {
    Setting *s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_settings() {
    for (Setting *s = committed; s; s = s->next) {
        printf("%s=%s\n", s->key, s->value);
    }
}

int main() {
    char cmd[20], key[100], value[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%99s", key);
            fgets(value, sizeof(value), stdin);
            while (value[0] == ' ') memmove(value, value + 1, strlen(value));
            value[strcspn(value, "\n")] = 0;
            if (in_transaction) set_setting(&staged, key, value);
            else set_setting(&committed, key, value);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            scanf("%99s", key);
            fgets(value, sizeof(value), stdin);
            while (value[0] == ' ') memmove(value, value + 1, strlen(value));
            value[strcspn(value, "\n")] = 0;
            set_setting(&staged, key, value);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%99s", key);
            get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    return 0;
}