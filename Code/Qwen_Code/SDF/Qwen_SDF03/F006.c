// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *name;
    int count;
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
        if (strcmp(cmd, "ADD") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                int c = atoi(ptr);
                Node *n = find_node(name);
                if (!n) {
                    n = malloc(sizeof(Node));
                    if (n) { n->name = strdup(name); n->count = c; n->next = head; head = n; }
                } else n->count += c;
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                int c = atoi(ptr);
                Node *n = find_node(name);
                if (n) { n->count -= c; if (n->count < 0) n->count = 0; }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Node *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->name); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Node *n = find_node(name);
                printf("%d\n", n ? n->count : 0);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Node *n = head; n; n = n->next) printf("%s %d\n", n->name, n->count);
        }
    }
    while (head) {
        Node *next = head->next;
        free(head->name); free(head);
        head = next;
    }
    return 0;
}