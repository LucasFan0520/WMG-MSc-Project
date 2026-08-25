// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;
int count = 0;

Line *get_node(int idx) {
    Line *p = head;
    for (int i = 0; i < idx && p; i++) {
        p = p->next;
    }
    return p;
}

void insert_line(int idx, const char *text) {
    if (idx < 0 || idx > count) {
        return;
    }
    Line *n = malloc(sizeof(Line));
    n->text = strdup(text);
    if (idx == 0) {
        n->next = head;
        head = n;
    } else {
        Line *p = get_node(idx - 1);
        n->next = p->next;
        p->next = n;
    }
    count++;
}

void append_line(const char *text) {
    insert_line(count, text);
}

void delete_line(int idx) {
    if (idx < 0 || idx >= count) {
        return;
    }
    Line *p;
    if (idx == 0) {
        p = head;
        head = head->next;
    } else {
        Line *prev = get_node(idx - 1);
        p = prev->next;
        prev->next = p->next;
    }
    free(p->text);
    free(p);
    count--;
}

void patch_line(int idx, const char *text) {
    if (idx < 0 || idx >= count) {
        return;
    }
    Line *p = get_node(idx);
    free(p->text);
    p->text = strdup(text);
}

void print_lines() {
    for (Line *p = head; p; p = p->next) {
        for (char *c = p->text; *c; c++) {
            if (*c == ' ') {
                putchar('_');
            } else {
                putchar(*c);
            }
        }
        putchar('\n');
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char text[9000];
        int idx;
        text[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "INSERT") == 0) {
            sscanf(line, "%*s %d %[^\n]", &idx, text);
            insert_line(idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            sscanf(line, "%*s %[^\n]", text);
            append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %d", &idx);
            delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            sscanf(line, "%*s %d %[^\n]", &idx, text);
            patch_line(idx, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    while (head) {
        Line *n = head->next;
        free(head->text);
        free(head);
        head = n;
    }
    return 0;
}