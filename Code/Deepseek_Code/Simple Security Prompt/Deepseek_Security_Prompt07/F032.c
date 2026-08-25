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
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        s->key = strdup(key);
        s->value = strdup(value);
        s->next = committed;
        committed = s;
    }
}

void unset_committed(const char *key) {
    Setting *prev = NULL;
    Setting *s = committed;
    while (s) {
        if (strcmp(s->key, key) == 0) {
            if (prev) prev->next = s->next;
            else committed = s->next;
            free(s->key);
            free(s->value);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void tset(const char *key, const char *value) {
    Setting *s = find_setting(staged, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        s->key = strdup(key);
        s->value = strdup(value);
        s->next = staged;
        staged = s;
    }
}

void commit_staged() {
    Setting *s = staged;
    while (s) {
        set_committed(s->key, s->value);
        s = s->next;
    }
    // clear staged
    Setting *cur = staged;
    while (cur) {
        Setting *next = cur->next;
        free(cur->key);
        free(cur->value);
        free(cur);
        cur = next;
    }
    staged = NULL;
}

void abort_staged() {
    Setting *cur = staged;
    while (cur) {
        Setting *next = cur->next;
        free(cur->key);
        free(cur->value);
        free(cur);
        cur = next;
    }
    staged = NULL;
}

void get_setting(const char *key) {
    Setting *s = find_setting(staged, key);
    if (!s) s = find_setting(committed, key);
    if (!s) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *p = s->value; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

void dump_committed() {
    Setting *s = committed;
    while (s) {
        printf("%s=", s->key);
        for (char *p = s->value; *p; p++) putchar(*p == ' ' ? '_' : *p);
        putchar('\n');
        s = s->next;
    }
}

void free_all() {
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "SET") == 0) {
            char key[256], value[4096];
            if (sscanf(rest, "%255s %[^\n]", key, value) == 2) {
                set_committed(key, value);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            // start transaction: clear any previous staged? We just clear staged if any.
            abort_staged();
        } else if (strcmp(cmd, "TSET") == 0) {
            char key[256], value[4096];
            if (sscanf(rest, "%255s %[^\n]", key, value) == 2) {
                tset(key, value);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_staged();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_staged();
        } else if (strcmp(cmd, "GET") == 0) {
            get_setting(rest);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_committed();
        }
    }
    free_all();
    return 0;
}