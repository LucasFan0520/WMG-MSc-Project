// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct Node { char *name; char *note; struct Node *next; } Node;
Node *head = NULL, *tail = NULL;

void join_q(const char *name, const char *note) {
    Node *n = malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(1); }
    n->name = strdup(name);
    n->note = strdup(note);
    if (!n->name || !n->note) { perror("strdup"); exit(1); }
    n->next = NULL;
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

void serve_q() {
    if (!head) return;
    printf("%s\n", head->name);
    Node *tmp = head;
    head = head->next;
    if (!head) tail = NULL;
    free(tmp->name); free(tmp->note);
    free(tmp);
}

void cancel_q(const char *name) {
    Node **curr = &head;
    Node *prev = NULL;
    while (*curr) {
        if (strcmp((*curr)->name, name) == 0) {
            Node *tmp = *curr;
            *curr = tmp->next;
            if (!*curr) tail = prev;
            free(tmp->name); free(tmp->note);
            free(tmp);
            return;
        }
        prev = *curr;
        curr = &(*curr)->next;
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) != 1) { free(line); continue; }
            char *note = strchr(line, ' ');
            if (note) { note = strchr(note + 1, ' '); if (note) note++; }
            if (!note) note = "";
            join_q(name, note);
        } else if (strcmp(cmd, "SERVE") == 0) {
            serve_q();
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) cancel_q(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                for (Node *n = head; n; n = n->next) {
                    if (strcmp(n->name, name) == 0) { printf("%s\n", n->note); break; }
                }
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (Node *n = head; n; n = n->next) printf("%s\n", n->name);
        }
        free(line);
    }
    while (head) {
        Node *tmp = head;
        head = head->next;
        free(tmp->name); free(tmp->note);
        free(tmp);
    }
    return 0;
}