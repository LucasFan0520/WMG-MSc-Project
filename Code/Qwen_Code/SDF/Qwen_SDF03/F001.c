// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *id;
    char *text;
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

Node *find_node(const char *id) {
    for (Node *n = head; n; n = n->next)
        if (strcmp(n->id, id) == 0) return n;
    return NULL;
}

void append_text(Node *n, const char *text) {
    size_t len = strlen(n->text) + strlen(text) + 2;
    char *buf = malloc(len);
    if (!buf) return;
    snprintf(buf, len, "%s %s", n->text, text);
    free(n->text);
    n->text = buf;
}

int main() {
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        char *id = get_token(&ptr);
        char *text = ptr;
        
        if (strcmp(cmd, "NEW") == 0 && id) {
            if (!find_node(id)) {
                Node *node = malloc(sizeof(Node));
                if (node) {
                    node->id = strdup(id);
                    node->text = strdup(text);
                    if (!node->id || !node->text) {
                        free(node->id); free(node->text); free(node); continue;
                    }
                    node->next = head;
                    head = node;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0 && id) {
            Node *node = find_node(id);
            if (node) append_text(node, text);
        } else if (strcmp(cmd, "REPLACE") == 0 && id) {
            Node *node = find_node(id);
            if (node) {
                char *tmp = strdup(text);
                if (tmp) { free(node->text); node->text = tmp; }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && id) {
            Node *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->id, id) == 0) {
                    if (prev) prev->next = curr->next;
                    else head = curr->next;
                    free(curr->id); free(curr->text); free(curr);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0 && id) {
            Node *node = find_node(id);
            if (!node) printf("NOT_FOUND\n");
            else {
                for (char *p = node->text; *p; p++)
                    putchar(*p == ' ' ? '_' : *p);
                putchar('\n');
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Node *n = head; n; n = n->next)
                printf("%s\n", n->id);
        }
    }
    while (head) {
        Node *next = head->next;
        free(head->id); free(head->text); free(head);
        head = next;
    }
    return 0;
}