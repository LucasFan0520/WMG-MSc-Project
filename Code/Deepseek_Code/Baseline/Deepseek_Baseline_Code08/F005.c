// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct LineNode {
    char *text;
    struct LineNode *next;
} LineNode;

LineNode *head = NULL;
int line_count = 0;

LineNode* get_node(int index) {
    int i = 0;
    LineNode *cur = head;
    while (cur && i < index) { cur = cur->next; i++; }
    return (i == index) ? cur : NULL;
}

void insert_line(int index, const char *text) {
    if (index < 0) return;
    LineNode *new = malloc(sizeof(LineNode));
    new->text = strdup(text);
    new->next = NULL;
    if (index == 0) {
        new->next = head;
        head = new;
    } else {
        LineNode *prev = get_node(index - 1);
        if (!prev) { free(new->text); free(new); return; }
        new->next = prev->next;
        prev->next = new;
    }
    line_count++;
}

void append_line(const char *text) {
    LineNode *new = malloc(sizeof(LineNode));
    new->text = strdup(text);
    new->next = NULL;
    if (!head) head = new;
    else {
        LineNode *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
    line_count++;
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    LineNode *prev = NULL, *cur = head;
    int i = 0;
    while (cur && i < index) { prev = cur; cur = cur->next; i++; }
    if (!cur) return;
    if (prev) prev->next = cur->next;
    else head = cur->next;
    free(cur->text);
    free(cur);
    line_count--;
}

void patch_line(int index, const char *text) {
    LineNode *n = get_node(index);
    if (!n) return;
    free(n->text);
    n->text = strdup(text);
}

void free_all() {
    LineNode *cur = head;
    while (cur) {
        LineNode *tmp = cur;
        cur = cur->next;
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
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = strtok(NULL, " ");
            if (!idx_str) continue;
            int idx = atoi(idx_str);
            char *text = strtok(NULL, "");
            if (!text) text = "";
            insert_line(idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = strtok(NULL, "");
            if (!text) text = "";
            append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_str = strtok(NULL, " ");
            if (!idx_str) continue;
            int idx = atoi(idx_str);
            delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_str = strtok(NULL, " ");
            if (!idx_str) continue;
            int idx = atoi(idx_str);
            char *text = strtok(NULL, "");
            if (!text) text = "";
            patch_line(idx, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            LineNode *cur = head;
            while (cur) {
                char *p = cur->text;
                while (*p) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                    p++;
                }
                putchar('\n');
                cur = cur->next;
            }
        }
    }
    free_all();
    return 0;
}