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

Node *find(const char *id) {
    for (Node *n = head; n; n = n->next)
        if (strcmp(n->id, id) == 0) return n;
    return NULL;
}

void add(const char *id, const char *text) {
    Node *n = find(id);
    if (n) {
        free(n->text);
        n->text = strdup(text);
    } else {
        n = malloc(sizeof(Node));
        n->id = strdup(id);
        n->text = strdup(text);
        n->next = NULL;
        Node **p = &head;
        while (*p) p = &(*p)->next;
        *p = n;
    }
}

void append(const char *id, const char *text) {
    Node *n = find(id);
    if (n) {
        size_t len = strlen(n->text) + strlen(text) + 2;
        char *buf = malloc(len);
        snprintf(buf, len, "%s %s", n->text, text);
        free(n->text);
        n->text = buf;
    } else {
        add(id, text);
    }
}

void delete_node(const char *id) {
    Node **p = &head;
    while (*p) {
        if (strcmp((*p)->id, id) == 0) {
            Node *tmp = *p;
            *p = tmp->next;
            free(tmp->id);
            free(tmp->text);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void show(const char *id) {
    Node *n = find(id);
    if (!n) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = n->text; *c; c++)
        putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void list() {
    for (Node *n = head; n; n = n->next)
        printf("%s\n", n->id);
}

void cleanup() {
    while (head) {
        Node *tmp = head->next;
        free(head->id);
        free(head->text);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256], text[8192] = "";
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "REPLACE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) strcpy(text, t + 1); }
                add(id, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) strcpy(text, t + 1); }
                append(id, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) delete_node(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1) show(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list();
        }
    }
    cleanup();
    return 0;
}