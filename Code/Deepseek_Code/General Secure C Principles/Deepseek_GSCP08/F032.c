// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Setting { char *key; char *val; struct Setting *next; } Setting;
Setting *committed = NULL;
Setting *staged = NULL;

Setting *find_setting(Setting *list, const char *key) {
    for (Setting *s = list; s; s = s->next)
        if (strcmp(s->key, key) == 0) return s;
    return NULL;
}

void set_setting(Setting **list, const char *key, const char *val) {
    Setting *s = find_setting(*list, key);
    if (s) { free(s->val); s->val = strdup(val); return; }
    s = malloc(sizeof(Setting));
    if (!s) return;
    s->key = strdup(key);
    s->val = strdup(val);
    s->next = *list;
    *list = s;
}

void unset_setting(Setting **list, const char *key) {
    Setting *prev = NULL;
    for (Setting *s = *list; s; prev = s, s = s->next) {
        if (strcmp(s->key, key) == 0) {
            if (prev) prev->next = s->next; else *list = s->next;
            free(s->key); free(s->val); free(s);
            return;
        }
    }
}

void get_setting(const char *key) {
    Setting *s = find_setting(committed, key);
    if (s) { printf("%s\n", s->val); }
}

void dump_settings(void) {
    for (Setting *s = committed; s; s = s->next)
        printf("%s %s\n", s->key, s->val);
}

int main() {
    int in_trans = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s", key) == 1) {
                char *v = rest + strlen(key);
                while (*v == ' ') v++;
                strcpy(val, v);
                set_setting(&committed, key, val);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            if (!in_trans) {
                in_trans = 1;
                // clear staged
                while (staged) { Setting *s = staged; staged = s->next; free(s->key); free(s->val); free(s); }
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            if (!in_trans) continue;
            char key[256], val[4096];
            if (sscanf(rest, "%255s", key) == 1) {
                char *v = rest + strlen(key);
                while (*v == ' ') v++;
                strcpy(val, v);
                set_setting(&staged, key, val);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                // apply staged to committed
                for (Setting *s = staged; s; s = s->next) {
                    set_setting(&committed, s->key, s->val);
                }
                // clear staged
                while (staged) { Setting *s = staged; staged = s->next; free(s->key); free(s->val); free(s); }
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                while (staged) { Setting *s = staged; staged = s->next; free(s->key); free(s->val); free(s); }
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) get_setting(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_settings();
        }
    }
    while (committed) { Setting *s = committed; committed = s->next; free(s->key); free(s->val); free(s); }
    while (staged) { Setting *s = staged; staged = s->next; free(s->key); free(s->val); free(s); }
    return 0;
}