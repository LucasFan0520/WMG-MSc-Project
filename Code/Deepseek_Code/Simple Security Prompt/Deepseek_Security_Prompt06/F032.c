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

void set_committed(const char *key, const char *value) {
    Setting *s = find_setting(committed, key);
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

void tset(const char *key, const char *value) {
    Setting *s = find_setting(staged, key);
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

void commit_staged(void) {
    Setting *s = staged;
    while (s) {
        set_committed(s->key, s->value);
        s = s->next;
    }
    // clear staged
    s = staged;
    while (s) {
        Setting *tmp = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = tmp;
    }
    staged = NULL;
}

void abort_staged(void) {
    Setting *s = staged;
    while (s) {
        Setting *tmp = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = tmp;
    }
    staged = NULL;
}

void get_key(const char *key) {
    Setting *s = find_setting(staged, key);
    if (s) {
        printf("%s\n", s->value);
        return;
    }
    s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

void dump_committed(void) {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void free_settings(Setting *list) {
    Setting *s = list;
    while (s) {
        Setting *tmp = s->next;
        free(s->key);
        free(s->value);
        free(s);
        s = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(rest, "%255s", key) == 1) {
                const char *v = rest + strlen(key) + 1;
                while (*v == ' ') v++;
                if (*v) set_committed(key, v);
                else set_committed(key, "");
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            // nothing
        } else if (strcmp(cmd, "TSET") == 0) {
            if (sscanf(rest, "%255s", key) == 1) {
                const char *v = rest + strlen(key) + 1;
                while (*v == ' ') v++;
                if (*v) tset(key, v);
                else tset(key, "");
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_staged();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_staged();
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(rest, "%255s", key) == 1) get_key(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_committed();
        }
    }
    free_settings(committed);
    free_settings(staged);
    return 0;
}