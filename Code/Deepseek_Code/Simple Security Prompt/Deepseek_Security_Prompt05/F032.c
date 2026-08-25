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

void set_immediate(const char *key, const char *value) {
    Setting *s = find_setting(committed, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        if (!s) return;
        s->key = strdup(key);
        s->value = strdup(value);
        s->next = committed;
        committed = s;
    }
}

void begin_transaction(void) {
    if (transaction_active) return;
    transaction_active = 1;
    staged = NULL;
}

void tset_staged(const char *key, const char *value) {
    if (!transaction_active) return;
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        if (!s) return;
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
        set_immediate(s->key, s->value);
        s = s->next;
    }
    while (staged) {
        Setting *tmp = staged;
        staged = staged->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    transaction_active = 0;
}

void abort_transaction(void) {
    if (!transaction_active) return;
    while (staged) {
        Setting *tmp = staged;
        staged = staged->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    transaction_active = 0;
}

void get_setting(const char *key) {
    Setting *s = find_setting(committed, key);
    if (!s) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", s->value);
}

void dump_all(void) {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (strcmp(cmd, "SET") == 0 && n >= 3) {
            set_immediate(arg1, arg2);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0 && n >= 3) {
            tset_staged(arg1, arg2);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0 && n >= 2) {
            get_setting(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_all();
        }
    }
    while (committed) {
        Setting *tmp = committed;
        committed = committed->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    while (staged) {
        Setting *tmp = staged;
        staged = staged->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    return 0;
}