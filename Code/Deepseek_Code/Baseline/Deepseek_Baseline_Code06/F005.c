/* F005.c */
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
    if (index < 0 || index >= line_count) return NULL;
    LineNode *p = head;
    for (int i = 0; i < index; i++) p = p->next;
    return p;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    LineNode *new = malloc(sizeof(LineNode));
    new->text = strdup(text);
    if (index == 0) {
        new->next = head;
        head = new;
    } else {
        LineNode *prev = get_node(index - 1);
        new->next = prev->next;
        prev->next = new;
    }
    line_count++;
}

void append_line(const char *text) {
    insert_line(line_count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    LineNode *p = head, *prev = NULL;
    for (int i = 0; i < index; i++) { prev = p; p = p->next; }
    if (prev) prev->next = p->next;
    else head = p->next;
    free(p->text);
    free(p);
    line_count--;
}

void patch_line(int index, const char *text) {
    LineNode *p = get_node(index);
    if (!p) return;
    free(p->text);
    p->text = strdup(text);
}

void print_lines() {
    LineNode *p = head;
    while (p) {
        for (char *c = p->text; *c; c++) {
            if (*c == ' ') putchar('_');
            else putchar(*c);
        }
        putchar('\n');
        p = p->next;
    }
}

void free_all() {
    while (head) {
        LineNode *tmp = head;
        head = head->next;
        free(tmp->text);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; int index; char text[4096];
        int n = sscanf(line, "%31s %d %[^\n]", cmd, &index, text);
        if (strcmp(cmd, "INSERT") == 0 && n == 3) {
            insert_line(index, text);
        } else if (strcmp(cmd, "APPEND") == 0 && n == 2) {
            char t[4096];
            sscanf(line, "%*s %[^\n]", t);
            append_line(t);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_line(index);
        } else if (strcmp(cmd, "PATCH") == 0 && n == 3) {
            patch_line(index, text);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_lines();
        }
    }
    free_all();
    return 0;
}