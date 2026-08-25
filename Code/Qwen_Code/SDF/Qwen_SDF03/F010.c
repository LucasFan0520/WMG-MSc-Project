// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Key {
    char *name;
    char **vals;
    size_t count, cap;
    struct Key *next;
} Key;

Key *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Key *find_key(const char *name) {
    for (Key *k = head; k; k = k->next)
        if (strcmp(k->name, name) == 0) return k;
    return NULL;
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Key *k = find_key(name);
                if (!k) {
                    k = malloc(sizeof(Key));
                    if (k) { k->name = strdup(name); k->vals = NULL; k->count = k->cap = 0; k->next = head; head = k; }
                }
                if (k && k->count == k->cap) {
                    size_t new_cap = k->cap ? k->cap * 2 : 4;
                    char **tmp = realloc(k->vals, new_cap * sizeof(char *));
                    if (tmp) { k->vals = tmp; k->cap = new_cap; }
                }
                if (k) k->vals[k->count++] = strdup(ptr);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Key *k = find_key(name);
                if (k) {
                    for (size_t i = 0; i < k->count; i++) {
                        if (strcmp(k->vals[i], ptr) == 0) {
                            free(k->vals[i]);
                            k->vals[i] = k->vals[--k->count];
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Key *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        for (size_t i = 0; i < curr->count; i++) free(curr->vals[i]);
                        free(curr->vals); free(curr->name); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Key *k = find_key(name);
                if (k) for (size_t i = 0; i < k->count; i++) printf("%s\n", k->vals[i]);
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (Key *k = head; k; k = k->next) printf("%s\n", k->name);
        }
    }
    while (head) {
        Key *next = head->next;
        for (size_t i = 0; i < head->count; i++) free(head->vals[i]);
        free(head->vals); free(head->name); free(head);
        head = next;
    }
    return 0;
}