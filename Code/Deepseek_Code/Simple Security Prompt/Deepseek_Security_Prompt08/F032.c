// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Setting {
    char *key;
    char *value;
    struct Setting *next;
} Setting;

Setting *committed = NULL, *staged = NULL;

Setting *find_in_committed(const char *key) {
    Setting *s = committed;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

Setting *find_in_staged(const char *key) {
    Setting *s = staged;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void set_committed(const char *key, const char *value) {
    Setting *s = find_in_committed(key);
    if (s) {
        free(s->value);
        s->value = malloc(strlen(value) + 1);
        strcpy(s->value, value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = malloc(strlen(key) + 1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value) + 1);
        strcpy(new->value, value);
        new->next = committed;
        committed = new;
    }
}

void set_staged(const char *key, const char *value) {
    Setting *s = find_in_staged(key);
    if (s) {
        free(s->value);
        s->value = malloc(strlen(value) + 1);
        strcpy(s->value, value);
    } else {
        Setting *new = malloc(sizeof(Setting));
        new->key = malloc(strlen(key) + 1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value) + 1);
        strcpy(new->value, value);
        new->next = staged;
        staged = new;
    }
}

void begin_transaction(void) {
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
        set_committed(s->key, s->value);
        s = s->next;
    }
    Setting *old = staged;
    staged = NULL;
    s = old;
    while (s) {
        Setting *next = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = next;
    }
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

void get_setting(const char *key) {
    Setting *s = find_in_committed(key);
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");
            if (key && value) set_committed(key, value);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");
            if (key && value) set_staged(key, value);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    free(line);
    free_all();
    return 0;
}