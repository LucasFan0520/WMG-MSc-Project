// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Val { char *text; struct Val *next; } Val;
typedef struct Key { char *name; Val *vals; struct Key *next; } Key;
Key *head = NULL;

Key *find_key(const char *name) {
    for (Key *k = head; k; k = k->next) if (strcmp(k->name, name) == 0) return k;
    return NULL;
}

void add_val(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        k->name = strdup(key);
        k->vals = NULL;
        k->next = head;
        head = k;
    }
    Val *v = malloc(sizeof(Val));
    v->text = strdup(val);
    v->next = NULL;
    if (!k->vals) k->vals = v;
    else {
        Val *p = k->vals;
        while (p->next) p = p->next;
        p->next = v;
    }
}

void remove_val(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) return;
    Val **p = &k->vals;
    while (*p) {
        if (strcmp((*p)->text, val) == 0) {
            Val *tmp = *p;
            *p = tmp->next;
            free(tmp->text);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void erase_key(const char *key) {
    Key **p = &head;
    while (*p) {
        if (strcmp((*p)->name, key) == 0) {
            Key *tmp = *p;
            *p = tmp->next;
            while (tmp->vals) {
                Val *v = tmp->vals->next;
                free(tmp->vals->text);
                free(tmp->vals);
                tmp->vals = v;
            }
            free(tmp->name);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void values_key(const char *key) {
    Key *k = find_key(key);
    if (k) for (Val *v = k->vals; v; v = v->next) printf("%s\n", v->text);
}

void keys() {
    for (Key *k = head; k; k = k->next) printf("%s\n", k->name);
}

void cleanup() {
    while (head) {
        Key *tmp = head->next;
        while (head->vals) {
            Val *v = head->vals->next;
            free(head->vals->text);
            free(head->vals);
            head->vals = v;
        }
        free(head->name);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) add_val(a1, a2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) remove_val(a1, a2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) erase_key(a1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) values_key(a1);
        } else if (strcmp(cmd, "KEYS") == 0) keys();
    }
    cleanup();
    return 0;
}