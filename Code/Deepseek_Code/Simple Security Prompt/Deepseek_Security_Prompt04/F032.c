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

Setting *find_setting(Setting *list, char *key) {
    for (Setting *s = list; s; s = s->next)
        if (strcmp(s->key, key) == 0) return s;
    return NULL;
}

void set_committed(char *key, char *value) {
    Setting *s = find_setting(committed, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = committed;
        committed = new;
    }
}

void begin_transaction(void) {
    // clear staged
    Setting *s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    staged = NULL;
}

void tset(char *key, char *value) {
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = staged;
        staged = new;
    }
}

void commit(void) {
    // apply staged to committed
    for (Setting *s = staged; s; s = s->next) {
        set_committed(s->key, s->value);
    }
    // clear staged
    Setting *s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    staged = NULL;
}

void abort_transaction(void) {
    Setting *s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    staged = NULL;
}

void get_key(char *key) {
    Setting *s = find_setting(staged, key);
    if (!s) s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_all(void) {
    for (Setting *s = committed; s; s = s->next)
        printf("%s %s\n", s->key, s->value);
}

void free_setting_list(Setting *list) {
    Setting *s = list;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
}

void free_all(void) {
    free_setting_list(committed);
    free_setting_list(staged);
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "SET") == 0 && n == 3) {
            set_committed(arg1, arg2);
        } else if (strcmp(cmd, "BEGIN") == 0 && n == 1) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0 && n == 3) {
            tset(arg1, arg2);
        } else if (strcmp(cmd, "COMMIT") == 0 && n == 1) {
            commit();
        } else if (strcmp(cmd, "ABORT") == 0 && n == 1) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0 && n == 2) {
            get_key(arg1);
        } else if (strcmp(cmd, "DUMP") == 0 && n == 1) {
            dump_all();
        }
    }
    free(line);
    free_all();
    return 0;
}