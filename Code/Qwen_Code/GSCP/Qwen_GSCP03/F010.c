// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Val {
    char *text;
    struct Val *next;
} Val;

typedef struct Key {
    char *name;
    Val *vals;
    struct Key *next;
} Key;

Key *head = NULL;

void free_vals(Val *v) {
    while (v) { Val *t = v; v = v->next; free(t->text); free(t); }
}

void free_list() {
    while (head) {
        Key *t = head; head = head->next;
        free(t->name); free_vals(t->vals); free(t);
    }
}

Key *find_key(const char *name) {
    for (Key *k = head; k; k = k->next)
        if (strcmp(k->name, name) == 0) return k;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], k1[256], v1[256];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s", k1, v1) != 2) continue;
            Key *k = find_key(k1);
            if (!k) {
                k = malloc(sizeof(Key));
                k->name = strdup(k1); k->vals = NULL; k->next = head; head = k;
            }
            Val *v = malloc(sizeof(Val));
            v->text = strdup(v1); v->next = k->vals; k->vals = v;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s %255s", k1, v1) != 2) continue;
            Key *k = find_key(k1);
            if (k) {
                Val *prev = NULL, *curr = k->vals;
                while (curr) {
                    if (strcmp(curr->text, v1) == 0) {
                        Val *tmp = curr; curr = curr->next;
                        if (prev) prev->next = curr; else k->vals = curr;
                        free(tmp->text); free(tmp);
                        break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", k1) != 1) continue;
            Key *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, k1) == 0) {
                    Key *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free_vals(tmp->vals); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (sscanf(line, "%*s %255s", k1) != 1) continue;
            Key *k = find_key(k1);
            if (k) for (Val *v = k->vals; v; v = v->next) printf("%s\n", v->text);
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (Key *k = head; k; k = k->next) printf("%s\n", k->name);
        }
    }
    free_list();
    return 0;
}