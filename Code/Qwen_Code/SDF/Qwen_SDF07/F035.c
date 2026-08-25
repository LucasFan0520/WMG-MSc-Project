// F035.c
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

typedef struct Node { char *name; char *payload; struct Node *next; } Node;
Node *head = NULL, *tail = NULL;

void enqueue(const char *name, const char *payload) {
    Node *n = malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(1); }
    n->name = strdup(name);
    n->payload = strdup(payload);
    if (!n->name || !n->payload) { perror("strdup"); exit(1); }
    n->next = NULL;
    if (tail) tail->next = n;
    else head = n;
    tail = n;
}

void clone_job(const char *old, const char *new_name) {
    for (Node *n = head; n; n = n->next) {
        if (strcmp(n->name, old) == 0) {
            enqueue(new_name, n->payload);
            return;
        }
    }
}

void cancel_job(const char *name) {
    Node **curr = &head;
    Node *prev = NULL;
    while (*curr) {
        if (strcmp((*curr)->name, name) == 0) {
            Node *tmp = *curr;
            *curr = tmp->next;
            if (!*curr) tail = prev;
            free(tmp->name); free(tmp->payload);
            free(tmp);
            return;
        }
        prev = *curr;
        curr = &(*curr)->next;
    }
}

void run_job(const char *name) {
    Node **curr = &head;
    Node *prev = NULL;
    while (*curr) {
        if (strcmp((*curr)->name, name) == 0) {
            Node *tmp = *curr;
            printf("%s %s\n", tmp->name, tmp->payload);
            *curr = tmp->next;
            if (!*curr) tail = prev;
            free(tmp->name); free(tmp->payload);
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
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) != 1) { free(line); continue; }
            char *payload = strchr(line, ' ');
            if (payload) { payload = strchr(payload + 1, ' '); if (payload) payload++; }
            if (!payload) payload = "";
            enqueue(name, payload);
        } else if (strcmp(cmd, "CLONE") == 0) {
            char old[256], new_name[256];
            if (sscanf(line, "%*s %255s %255s", old, new_name) == 2) clone_job(old, new_name);
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) cancel_job(name);
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) run_job(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Node *n = head; n; n = n->next) printf("%s %s\n", n->name, n->payload);
        }
        free(line);
    }
    while (head) {
        Node *tmp = head;
        head = head->next;
        free(tmp->name); free(tmp->payload);
        free(tmp);
    }
    return 0;
}