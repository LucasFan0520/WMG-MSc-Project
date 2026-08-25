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
        s->value = malloc(strlen(value)+1);
        strcpy(s->value, value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = malloc(strlen(key)+1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value)+1);
        strcpy(new->value, value);
        new->next = committed;
        committed = new;
    }
}

void tset_staged(const char *key, const char *value) {
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = malloc(strlen(value)+1);
        strcpy(s->value, value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = malloc(strlen(key)+1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value)+1);
        strcpy(new->value, value);
        new->next = staged;
        staged = new;
    }
}

void begin_transaction(void) {
    // discard previous staged
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

void commit_transaction(void) {
    Setting *s = staged;
    while (s) {
        set_immediate(s->key, s->value);
        s = s->next;
    }
    // clear staged
    s = staged;
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

void get_value(const char *key) {
    Setting *s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_settings(void) {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void free_all(void) {
    Setting *s = committed;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
    s = staged;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(rest, " ");
            if (!key) continue;
            char *value = strtok(NULL, "");
            if (!value) value = "";
            set_immediate(key, value);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            char *key = strtok(rest, " ");
            if (!key) continue;
            char *value = strtok(NULL, "");
            if (!value) value = "";
            tset_staged(key, value);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(rest, " ");
            if (key) get_value(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    free_all();
    return 0;
}