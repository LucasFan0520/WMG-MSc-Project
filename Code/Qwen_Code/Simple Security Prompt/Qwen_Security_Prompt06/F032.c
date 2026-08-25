// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Set {
    char *key;
    char *val;
    struct Set *next;
} Set;

Set *sets = NULL;
Set *stage = NULL;
int in_trans = 0;

Set *find_in(Set *list, const char *key) {
    for (Set *s = list; s; s = s->next) {
        if (strcmp(s->key, key) == 0) return s;
    }
    return NULL;
}

void set_val(Set **list, const char *key, const char *val) {
    Set *s = find_in(*list, key);
    if (s) {
        free(s->val);
        s->val = strdup(val);
    } else {
        s = malloc(sizeof(Set));
        s->key = strdup(key);
        s->val = strdup(val);
        s->next = *list;
        *list = s;
    }
}

void get_val(const char *key) {
    Set *s = find_in(sets, key);
    if (s) printf("%s\n", s->val);
}

void commit_trans() {
    Set *curr = stage;
    while (curr) {
        set_val(&sets, curr->key, curr->val);
        curr = curr->next;
    }
    curr = stage;
    while (curr) {
        Set *next = curr->next;
        free(curr->key);
        free(curr->val);
        free(curr);
        curr = next;
    }
    stage = NULL;
    in_trans = 0;
}

void abort_trans() {
    Set *curr = stage;
    while (curr) {
        Set *next = curr->next;
        free(curr->key);
        free(curr->val);
        free(curr);
        curr = next;
    }
    stage = NULL;
    in_trans = 0;
}

void dump_sets() {
    for (Set *s = sets; s; s = s->next) {
        printf("%s=%s\n", s->key, s->val);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0};
        sscanf(line, "%15s %255s", cmd, arg1);
        
        if (strcmp(cmd, "SET") == 0) {
            char *val = get_rest(line, 2);
            if (val) set_val(&sets, arg1, val);
        }
        else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
        }
        else if (strcmp(cmd, "TSET") == 0) {
            char *val = get_rest(line, 2);
            if (val && in_trans) set_val(&stage, arg1, val);
        }
        else if (strcmp(cmd, "COMMIT") == 0) commit_trans();
        else if (strcmp(cmd, "ABORT") == 0) abort_trans();
        else if (strcmp(cmd, "GET") == 0) get_val(arg1);
        else if (strcmp(cmd, "DUMP") == 0) dump_sets();
        
        free(line);
    }
    return 0;
}