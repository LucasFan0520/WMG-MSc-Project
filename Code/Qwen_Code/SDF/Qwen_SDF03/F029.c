// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Rec {
    char *key;
    char *value;
    struct Rec *next;
} Rec;

Rec *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                Rec *r = malloc(sizeof(Rec));
                if (r) {
                    r->key = strdup(key);
                    r->value = strdup(ptr);
                    r->next = head;
                    head = r;
                }
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            Rec *curr = head;
            while (curr) {
                Rec *prev = curr, *next = curr->next;
                while (next) {
                    if (strcmp(next->key, curr->key) == 0) {
                        prev->next = next->next;
                        free(next->key);
                        free(next->value);
                        free(next);
                        next = prev->next;
                    } else {
                        prev = next;
                        next = next->next;
                    }
                }
                curr = curr->next;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                Rec *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->key, key) == 0) {
                        Rec *next = curr->next;
                        if (prev) prev->next = next;
                        else head = next;
                        free(curr->key);
                        free(curr->value);
                        free(curr);
                        curr = next;
                    } else {
                        prev = curr;
                        curr = curr->next;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                for (Rec *r = head; r; r = r->next) {
                    if (strcmp(r->key, key) == 0) {
                        printf("%s\n", r->value);
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s %s\n", r->key, r->value);
            }
        }
    }
    while (head) {
        Rec *next = head->next;
        free(head->key);
        free(head->value);
        free(head);
        head = next;
    }
    return 0;
}