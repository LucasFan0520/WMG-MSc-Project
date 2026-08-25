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

Setting *find_setting(Setting *list, const char *key) {
    Setting *s = list;
    while (s) {
        if (strcmp(s->key, key) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void set_committed(const char *key, const char *value) {
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

void tset(const char *key, const char *value) {
    if (!in_transaction) return;
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

void begin_transaction(void) {
    if (in_transaction) return;
    in_transaction = 1;
}

void commit_transaction(void) {
    if (!in_transaction) return;
    Setting *s = staged;
    while (s) {
        set_committed(s->key, s->value);
        s = s->next;
    }
    // clear staged
    while (staged) {
        Setting *next = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = next;
    }
    in_transaction = 0;
}

void abort_transaction(void) {
    if (!in_transaction) return;
    while (staged) {
        Setting *next = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = next;
    }
    in_transaction = 0;
}

void get_setting(const char *key) {
    Setting *s = find_setting(committed, key);
    if (s) {
        for (char *p = s->value; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
    }
}

void dump_all(void) {
    Setting *s = committed;
    while (s) {
        printf("%s ", s->key);
        for (char *p = s->value; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        s = s->next;
    }
}

void free_all(void) {
    while (committed) {
        Setting *next = committed->next;
        free(committed->key);
        free(committed->value);
        free(committed);
        committed = next;
    }
    while (staged) {
        Setting *next = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "SET") == 0) {
            char key[256], value[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", key, value) == 2) {
                set_committed(key, value);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            char key[256], value[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", key, value) == 2) {
                tset(key, value);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                get_setting(key);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_all();
        }
    }

    free(line);
    free_all();
    return 0;
}