// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Set {
    char *key;
    char *value;
    struct Set *next;
} Set;

Set *head = NULL;
Set *staged = NULL;
int in_transaction = 0;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Set *find_in(Set *list, const char *key) {
    for (Set *s = list; s; s = s->next)
        if (strcmp(s->key, key) == 0) return s;
    return NULL;
}

void set_val(Set **list, const char *key, const char *val) {
    Set *s = find_in(*list, key);
    if (s) {
        char *tmp = strdup(val);
        if (tmp) {
            free(s->value);
            s->value = tmp;
        }
    } else {
        s = malloc(sizeof(Set));
        if (s) {
            s->key = strdup(key);
            s->value = strdup(val);
            s->next = *list;
            *list = s;
        }
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                if (in_transaction) {
                    set_val(&staged, key, ptr);
                } else {
                    set_val(&head, key, ptr);
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            char *key = get_token(&ptr);
            if (key && in_transaction) {
                set_val(&staged, key, ptr);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            while (staged) {
                Set *next = staged->next;
                set_val(&head, staged->key, staged->value);
                free(staged->key);
                free(staged->value);
                free(staged);
                staged = next;
            }
            in_transaction = 0;
        } else if (strcmp(cmd, "ABORT") == 0) {
            while (staged) {
                Set *next = staged->next;
                free(staged->key);
                free(staged->value);
                free(staged);
                staged = next;
            }
            in_transaction = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                Set *s = find_in(head, key);
                if (s) printf("%s\n", s->value);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Set *s = head; s; s = s->next) {
                printf("%s=%s\n", s->key, s->value);
            }
        }
    }
    while (head) {
        Set *next = head->next;
        free(head->key);
        free(head->value);
        free(head);
        head = next;
    }
    while (staged) {
        Set *next = staged->next;
        free(staged->key);
        free(staged->value);
        free(staged);
        staged = next;
    }
    return 0;
}