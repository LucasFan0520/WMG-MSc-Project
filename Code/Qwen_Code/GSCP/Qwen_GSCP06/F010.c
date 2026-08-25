// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Val {
    char* val;
    struct Val* next;
} Val;

typedef struct Key {
    char* key;
    Val* vals;
    struct Key* next;
} Key;

Key* head = NULL;

Key* find_key(const char* key) {
    for (Key* k = head; k; k = k->next) {
        if (strcmp(k->key, key) == 0) return k;
    }
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "ADD") == 0) {
            char* key = get_word(&p);
            Key* k = find_key(key);
            if (!k) {
                k = malloc(sizeof(Key));
                k->key = strdup(key);
                k->vals = NULL;
                k->next = head;
                head = k;
            }
            Val* v = malloc(sizeof(Val));
            v->val = strdup(p);
            v->next = NULL;
            if (!k->vals) k->vals = v;
            else {
                Val* tail = k->vals;
                while (tail->next) tail = tail->next;
                tail->next = v;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char* key = get_word(&p);
            Key* k = find_key(key);
            if (k) {
                Val* prev = NULL;
                for (Val* v = k->vals; v; prev = v, v = v->next) {
                    if (strcmp(v->val, p) == 0) {
                        if (prev) prev->next = v->next;
                        else k->vals = v->next;
                        free(v->val);
                        free(v);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char* key = get_word(&p);
            Key* prev = NULL;
            for (Key* k = head; prev = k, k = k->next) {
                if (strcmp(k->key, key) == 0) {
                    if (prev) prev->next = k->next;
                    else head = k->next;
                    free(k->key);
                    while (k->vals) {
                        Val* temp = k->vals;
                        k->vals = k->vals->next;
                        free(temp->val);
                        free(temp);
                    }
                    free(k);
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char* key = get_word(&p);
            Key* k = find_key(key);
            if (k) for (Val* v = k->vals; v; v = v->next) printf("%s\n", v->val);
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (Key* k = head; k; k = k->next) printf("%s\n", k->key);
        }
    }
    return 0;
}