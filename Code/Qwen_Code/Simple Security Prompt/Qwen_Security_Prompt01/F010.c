// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Val {
    char *v;
    struct Val *next;
} Val;

typedef struct Key {
    char *k;
    Val *vals;
    struct Key *next;
} Key;

Key *head = NULL;

Key *find_key(const char *k) {
    for (Key *p = head; p; p = p->next) {
        if (strcmp(p->k, k) == 0) {
            return p;
        }
    }
    return NULL;
}

void add_val(const char *k, const char *v) {
    Key *key = find_key(k);
    if (!key) {
        key = malloc(sizeof(Key));
        key->k = strdup(k);
        key->vals = NULL;
        key->next = head;
        head = key;
    }
    Val *val = malloc(sizeof(Val));
    val->v = strdup(v);
    val->next = key->vals;
    key->vals = val;
}

void remove_val(const char *k, const char *v) {
    Key *key = find_key(k);
    if (!key) {
        return;
    }
    Val *p = key->vals;
    Val *prev = NULL;
    while (p) {
        if (strcmp(p->v, v) == 0) {
            Val *next = p->next;
            free(p->v);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                key->vals = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void erase_key(const char *k) {
    Key *p = head;
    Key *prev = NULL;
    while (p) {
        if (strcmp(p->k, k) == 0) {
            Key *next = p->next;
            Val *v = p->vals;
            while (v) {
                Val *vn = v->next;
                free(v->v);
                free(v);
                v = vn;
            }
            free(p->k);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void values_key(const char *k) {
    Key *key = find_key(k);
    if (key) {
        for (Val *v = key->vals; v; v = v->next) {
            printf("%s\n", v->v);
        }
    }
}

void keys_list() {
    for (Key *p = head; p; p = p->next) {
        printf("%s\n", p->k);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char a[1000];
        char b[1000];
        b[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, a, b);
        if (strcmp(cmd, "ADD") == 0) {
            add_val(a, b);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_val(a, b);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_key(a);
        } else if (strcmp(cmd, "VALUES") == 0) {
            values_key(a);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    while (head) {
        Key *n = head->next;
        Val *v = head->vals;
        while (v) {
            Val *vn = v->next;
            free(v->v);
            free(v);
            v = vn;
        }
        free(head->k);
        free(head);
        head = n;
    }
    return 0;
}