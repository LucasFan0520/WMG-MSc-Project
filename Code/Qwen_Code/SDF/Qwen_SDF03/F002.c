// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *tag;
    char *msg;
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

Node *find_node(const char *tag) {
    for (Node *n = head; n; n = n->next)
        if (strcmp(n->tag, tag) == 0) return n;
    return NULL;
}

void set_msg(Node *n, const char *msg) {
    char *tmp = strdup(msg);
    if (tmp) { free(n->msg); n->msg = tmp; }
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
            char *tag = get_token(&ptr);
            if (tag) {
                Node *n = find_node(tag);
                if (!n) {
                    n = malloc(sizeof(Node));
                    if (n) { n->tag = strdup(tag); n->msg = strdup(ptr); n->next = head; head = n; }
                } else set_msg(n, ptr);
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *t1 = get_token(&ptr);
            char *t2 = get_token(&ptr);
            char *nt = get_token(&ptr);
            if (t1 && t2 && nt) {
                Node *n1 = find_node(t1), *n2 = find_node(t2);
                if (n1 && n2) {
                    size_t len = strlen(n1->msg) + strlen(n2->msg) + 2;
                    char *buf = malloc(len);
                    if (buf) {
                        snprintf(buf, len, "%s %s", n1->msg, n2->msg);
                        Node *nn = find_node(nt);
                        if (!nn) {
                            nn = malloc(sizeof(Node));
                            if (nn) { nn->tag = strdup(nt); nn->msg = buf; nn->next = head; head = nn; }
                            else free(buf);
                        } else { free(nn->msg); nn->msg = buf; }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = get_token(&ptr);
            if (tag) {
                Node *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->tag, tag) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->tag); free(curr->msg); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = get_token(&ptr);
            if (tag) {
                Node *n = find_node(tag);
                if (n) {
                    for (char *p = n->msg; *p; p++) putchar(*p == ' ' ? '_' : *p);
                    putchar('\n');
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Node *n = head; n; n = n->next) printf("%s %s\n", n->tag, n->msg);
        }
    }
    while (head) {
        Node *next = head->next;
        free(head->tag); free(head->msg); free(head);
        head = next;
    }
    return 0;
}