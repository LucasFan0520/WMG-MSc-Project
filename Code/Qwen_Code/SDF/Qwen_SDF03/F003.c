// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *name;
    char *exp;
    struct Node *next;
} Node;

Node *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Node *find_node(const char *name) {
    for (Node *n = head; n; n = n->next)
        if (strcmp(n->name, name) == 0) return n;
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
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Node *n = find_node(name);
                if (!n) {
                    n = malloc(sizeof(Node));
                    if (n) { n->name = strdup(name); n->exp = strdup(ptr); n->next = head; head = n; }
                } else { char *tmp = strdup(ptr); if (tmp) { free(n->exp); n->exp = tmp; } }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Node *n = find_node(name);
                if (n) {
                    for (char *p = n->exp; *p; p++) putchar(*p == ' ' ? '_' : *p);
                    putchar('\n');
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old = get_token(&ptr);
            char *newn = get_token(&ptr);
            if (old && newn) {
                Node *n = find_node(old);
                if (n && !find_node(newn)) {
                    char *tmp = strdup(newn);
                    if (tmp) { free(n->name); n->name = tmp; }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Node *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->name); free(curr->exp); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Node *n = head; n; n = n->next) printf("%s %s\n", n->name, n->exp);
        }
    }
    while (head) {
        Node *next = head->next;
        free(head->name); free(head->exp); free(head);
        head = next;
    }
    return 0;
}