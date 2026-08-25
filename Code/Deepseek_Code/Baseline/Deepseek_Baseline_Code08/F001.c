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

Node* find_node(const char *id) {
    Node *cur = head;
    while (cur) {
        if (strcmp(cur->id, id) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_node(const char *id, const char *text) {
    Node *new = malloc(sizeof(Node));
    new->id = strdup(id);
    new->text = strdup(text);
    new->next = NULL;
    if (!head) head = new;
    else {
        Node *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void free_all() {
    Node *cur = head;
    while (cur) {
        Node *tmp = cur;
        cur = cur->next;
        free(tmp->id);
        free(tmp->text);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            if (find_node(id)) continue;
            add_node(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            Node *n = find_node(id);
            if (!n) continue;
            size_t newlen = strlen(n->text) + strlen(text) + 1;
            char *newtext = malloc(newlen);
            strcpy(newtext, n->text);
            strcat(newtext, text);
            free(n->text);
            n->text = newtext;
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            Node *n = find_node(id);
            if (!n) continue;
            free(n->text);
            n->text = strdup(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            Node *prev = NULL, *cur = head;
            while (cur) {
                if (strcmp(cur->id, id) == 0) {
                    if (prev) prev->next = cur->next;
                    else head = cur->next;
                    free(cur->id);
                    free(cur->text);
                    free(cur);
                    break;
                }
                prev = cur;
                cur = cur->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            Node *n = find_node(id);
            if (!n) { printf("NOT_FOUND\n"); continue; }
            char *p = n->text;
            while (*p) {
                if (*p == ' ') putchar('_');
                else putchar(*p);
                p++;
            }
            putchar('\n');
        } else if (strcmp(cmd, "LIST") == 0) {
            Node *cur = head;
            while (cur) {
                printf("%s\n", cur->id);
                cur = cur->next;
            }
        }
    }
    free_all();
    return 0;
}