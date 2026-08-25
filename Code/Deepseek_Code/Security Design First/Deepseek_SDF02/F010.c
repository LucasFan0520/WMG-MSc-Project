/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct val {
    char *s;
    struct val *next;
} val_t;

typedef struct key {
    char *name;
    val_t *values;
    struct key *next;
} key_t;

key_t *keys = NULL;

key_t *find_key(const char *name) {
    key_t *k = keys;
    while (k) {
        if (strcmp(k->name, name) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(const char *keyname, const char *val) {
    key_t *k = find_key(keyname);
    if (!k) {
        k = malloc(sizeof(key_t));
        if (!k) return;
        k->name = strdup(keyname);
        k->values = NULL;
        k->next = keys;
        keys = k;
        if (!k->name) { free(k); return; }
    }
    val_t *v = malloc(sizeof(val_t));
    if (!v) return;
    v->s = strdup(val);
    v->next = NULL;
    if (!v->s) { free(v); return; }
    val_t **last = &k->values;
    while (*last) last = &(*last)->next;
    *last = v;
}

void remove_value(const char *keyname, const char *val) {
    key_t *k = find_key(keyname);
    if (!k) return;
    val_t *prev = NULL, *v = k->values;
    while (v) {
        if (strcmp(v->s, val) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v->s);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void erase_key(const char *keyname) {
    key_t *prev = NULL, *k = keys;
    while (k) {
        if (strcmp(k->name, keyname) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            val_t *v = k->values;
            while (v) {
                val_t *nxt = v->next;
                free(v->s);
                free(v);
                v = nxt;
            }
            free(k->name);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void values_key(const char *keyname) {
    key_t *k = find_key(keyname);
    if (!k) return;
    val_t *v = k->values;
    while (v) {
        printf("%s\n", v->s);
        v = v->next;
    }
}

void keys_list(void) {
    key_t *k = keys;
    while (k) {
        printf("%s\n", k->name);
        k = k->next;
    }
}

void free_all(void) {
    key_t *k = keys;
    while (k) {
        key_t *nxt = k->next;
        val_t *v = k->values;
        while (v) {
            val_t *n = v->next;
            free(v->s);
            free(v);
            v = n;
        }
        free(k->name);
        free(k);
        k = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        if (sscanf(line, "%9s %255s %255s", cmd, a, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "ADD") == 0) {
                add_value(a, rest);
            } else if (strcmp(cmd, "REMOVE") == 0) {
                remove_value(a, rest);
            } else if (strcmp(cmd, "ERASE") == 0) {
                erase_key(a);
            } else if (strcmp(cmd, "VALUES") == 0) {
                values_key(a);
            } else if (strcmp(cmd, "KEYS") == 0) {
                keys_list();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}