// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct val {
    char *value;
    struct val *next;
} Val;

typedef struct entry {
    char *key;
    Val *val_head;
    struct entry *next;
} Entry;

Entry *entries = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Entry *find_entry(const char *key) {
    Entry *e = entries;
    while (e) {
        if (strcmp(e->key, key) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void add_value(Entry *e, const char *val) {
    Val *v = malloc(sizeof(Val));
    if (!v) { fprintf(stderr, "Memory error\n"); exit(1); }
    v->value = strdup(val);
    if (!v->value) { fprintf(stderr, "Memory error\n"); exit(1); }
    v->next = e->val_head;
    e->val_head = v;
}

void add_key_value(const char *key, const char *val) {
    Entry *e = find_entry(key);
    if (!e) {
        e = malloc(sizeof(Entry));
        if (!e) { fprintf(stderr, "Memory error\n"); exit(1); }
        e->key = strdup(key);
        if (!e->key) { fprintf(stderr, "Memory error\n"); exit(1); }
        e->val_head = NULL;
        e->next = entries;
        entries = e;
    }
    add_value(e, val);
}

void remove_value(Entry *e, const char *val) {
    Val *v = e->val_head, *prev = NULL;
    while (v) {
        if (strcmp(v->value, val) == 0) {
            if (prev) prev->next = v->next;
            else e->val_head = v->next;
            free(v->value);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void remove_key_value(const char *key, const char *val) {
    Entry *e = find_entry(key);
    if (!e) return;
    remove_value(e, val);
}

void erase_key(const char *key) {
    Entry *e = entries, *prev = NULL;
    while (e) {
        if (strcmp(e->key, key) == 0) {
            if (prev) prev->next = e->next;
            else entries = e->next;
            free(e->key);
            Val *v = e->val_head;
            while (v) {
                Val *next = v->next;
                free(v->value);
                free(v);
                v = next;
            }
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void values_key(const char *key) {
    Entry *e = find_entry(key);
    if (!e) return;
    Val *v = e->val_head;
    int first = 1;
    while (v) {
        if (!first) printf(" ");
        for (char *s = v->value; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        first = 0;
        v = v->next;
    }
    putchar('\n');
}

void keys_list(void) {
    Entry *e = entries;
    int first = 1;
    while (e) {
        if (!first) printf(" ");
        printf("%s", e->key);
        first = 0;
        e = e->next;
    }
    printf("\n");
}

void free_all(void) {
    Entry *e = entries;
    while (e) {
        Entry *next = e->next;
        free(e->key);
        Val *v = e->val_head;
        while (v) {
            Val *nextv = v->next;
            free(v->value);
            free(v);
            v = nextv;
        }
        free(e);
        e = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "KEYS") == 0) { keys_list(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "REMOVE") == 0) {
            char *key = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            if (strcmp(cmd, "ADD") == 0) add_key_value(key, rest);
            else remove_key_value(key, rest);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_key(rest);
        } else if (strcmp(cmd, "VALUES") == 0) {
            values_key(rest);
        }
    }
    free_all();
    return 0;
}