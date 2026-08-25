// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *key;
    char *val;
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

Node *find_node(const char *key) {
    for (Node *n = head; n; n = n->next)
        if (strcmp(n->key, key) == 0) return n;
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
        if (strcmp(cmd, "SET") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                Node *n = find_node(key);
                if (!n) {
                    n = malloc(sizeof(Node));
                    if (n) { n->key = strdup(key); n->val = strdup(ptr); n->next = head; head = n; }
                } else { char *tmp = strdup(ptr); if (tmp) { free(n->val); n->val = tmp; } }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                Node *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->key, key) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->key); free(curr->val); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = ptr;
            while (*p) {
                if (*p == '{') {
                    char *end = strchr(p + 1, '}');
                    if (end) {
                        *end = '\0';
                        Node *n = find_node(p + 1);
                        if (n) printf("%s", n->val);
                        else printf("{%s}", p + 1);
                        *end = '}';
                        p = end + 1;
                    } else putchar(*p++);
                } else putchar(*p++);
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (Node *n = head; n; n = n->next) printf("%s=%s\n", n->key, n->val);
        }
    }
    while (head) {
        Node *next = head->next;
        free(head->key); free(head->val); free(head);
        head = next;
    }
    return 0;
}