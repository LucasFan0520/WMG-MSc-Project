/* F032.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Setting {
    char *key;
    char *value;
    struct Setting *next;
} Setting;

Setting *committed = NULL;
Setting *staged = NULL;

Setting *find_setting(Setting *list, const char *key) {
    while (list) {
        if (strcmp(list->key, key) == 0) return list;
        list = list->next;
    }
    return NULL;
}

void free_setting(Setting *s) {
    free(s->key);
    free(s->value);
    free(s);
}

void free_list(Setting *list) {
    while (list) {
        Setting *tmp = list;
        list = list->next;
        free_setting(tmp);
    }
}

void set_setting(Setting **list, const char *key, const char *value) {
    Setting *s = find_setting(*list, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        s = malloc(sizeof(Setting));
        s->key = strdup(key);
        s->value = strdup(value);
        s->next = *list;
        *list = s;
    }
}

void unset_setting(Setting **list, const char *key) {
    Setting *prev = NULL, *curr = *list;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else *list = curr->next;
            free_setting(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void get_setting(const char *key) {
    Setting *s = find_setting(committed, key);
    if (!s) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", s->value);
}

void dump_settings(void) {
    Setting *s = committed;
    while (s) {
        printf("%s %s\n", s->key, s->value);
        s = s->next;
    }
}

void begin_transaction(void) {
    free_list(staged);
    staged = NULL;
}

void tset(const char *key, const char *value) {
    set_setting(&staged, key, value);
}

void commit_transaction(void) {
    Setting *s = staged;
    while (s) {
        set_setting(&committed, s->key, s->value);
        s = s->next;
    }
    free_list(staged);
    staged = NULL;
}

void abort_transaction(void) {
    free_list(staged);
    staged = NULL;
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char key[256], value[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) != 1) continue;
            p += strlen(key);
            while (*p == ' ') p++;
            set_setting(&committed, key, p);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            if (!staged) continue;
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) != 1) continue;
            p += strlen(key);
            while (*p == ' ') p++;
            tset(key, p);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (staged) commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (staged) abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) == 1) get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }

    free_list(committed);
    free_list(staged);
    return 0;
}