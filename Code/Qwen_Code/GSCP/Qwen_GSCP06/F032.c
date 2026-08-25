// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Set {
    char* key;
    char* val;
    struct Set* next;
} Set;

Set* head = NULL;
Set* trans_head = NULL;
int in_trans = 0;

Set* find_set(Set* h, const char* key) {
    for (Set* s = h; s; s = s->next)
        if (strcmp(s->key, key) == 0) return s;
    return NULL;
}

void add_to_trans(const char* key, const char* val) {
    Set* s = find_set(trans_head, key);
    if (s) {
        free(s->val);
        s->val = strdup(val);
    } else {
        Set* n = malloc(sizeof(Set));
        n->key = strdup(key);
        n->val = strdup(val);
        n->next = trans_head;
        trans_head = n;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "SET") == 0) {
            char* key = get_word(&p);
            if (key) {
                Set* s = find_set(head, key);
                if (s) {
                    free(s->val);
                    s->val = strdup(p);
                } else {
                    Set* n = malloc(sizeof(Set));
                    n->key = strdup(key);
                    n->val = strdup(p);
                    n->next = head;
                    head = n;
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            char* key = get_word(&p);
            if (key && in_trans) {
                add_to_trans(key, p);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                for (Set* t = trans_head; t; t = t->next) {
                    Set* s = find_set(head, t->key);
                    if (s) {
                        free(s->val);
                        s->val = strdup(t->val);
                    } else {
                        Set* n = malloc(sizeof(Set));
                        n->key = strdup(t->key);
                        n->val = strdup(t->val);
                        n->next = head;
                        head = n;
                    }
                }
                while (trans_head) {
                    Set* temp = trans_head;
                    trans_head = trans_head->next;
                    free(temp->key);
                    free(temp->val);
                    free(temp);
                }
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                while (trans_head) {
                    Set* temp = trans_head;
                    trans_head = trans_head->next;
                    free(temp->key);
                    free(temp->val);
                    free(temp);
                }
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char* key = get_word(&p);
            Set* s = find_set(head, key);
            if (s) printf("%s\n", s->val);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Set* s = head; s; s = s->next) {
                printf("%s %s\n", s->key, s->val);
            }
        }
    }
    return 0;
}