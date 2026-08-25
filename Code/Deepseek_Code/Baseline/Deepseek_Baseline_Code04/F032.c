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
int in_transaction = 0;

Setting *find_setting(Setting *list, const char *key) {
    while (list) {
        if (strcmp(list->key, key) == 0) return list;
        list = list->next;
    }
    return NULL;
}

void set_setting(Setting **list, const char *key, const char *value) {
    Setting *s = find_setting(*list, key);
    if (s) {
        free(s->value);
        s->value = strdup(value);
    } else {
        Setting *ns = malloc(sizeof(Setting));
        ns->key = strdup(key);
        ns->value = strdup(value);
        ns->next = *list;
        *list = ns;
    }
}

void unset_setting(Setting **list, const char *key) {
    Setting *prev = NULL, *cur = *list;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else *list = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void free_list(Setting *list) {
    while (list) {
        Setting *tmp = list;
        list = list->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

void begin_transaction(void) {
    if (in_transaction) {
        // abort previous? we'll just discard staged
        free_list(staged);
        staged = NULL;
    }
    in_transaction = 1;
}

void commit_transaction(void) {
    if (!in_transaction) return;
    // apply staged to committed
    Setting *s = staged;
    while (s) {
        set_setting(&committed, s->key, s->value);
        s = s->next;
    }
    free_list(staged);
    staged = NULL;
    in_transaction = 0;
}

void abort_transaction(void) {
    if (!in_transaction) return;
    free_list(staged);
    staged = NULL;
    in_transaction = 0;
}

void tset(const char *key, const char *value) {
    if (!in_transaction) return;
    set_setting(&staged, key, value);
}

void get_setting(const char *key) {
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char key[256];
        int n = sscanf(line, "%s %s", cmd, key);
        if (strcmp(cmd, "SET") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(key) + 1;
            while (*p == ' ') p++;
            if (*p) set_setting(&committed, key, p);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            begin_transaction();
        } else if (strcmp(cmd, "TSET") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1 + strlen(key) + 1;
            while (*p == ' ') p++;
            if (*p) tset(key, p);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_transaction();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            if (n < 2) continue;
            get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    free_list(committed);
    free_list(staged);
    return 0;
}