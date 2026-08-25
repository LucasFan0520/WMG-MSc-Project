// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line { char *text; struct Line *next; } Line;
Line *head = NULL;
int count = 0;

void insert_line(int idx, const char *text) {
    if (idx < 0 || idx > count) return;
    Line *n = malloc(sizeof(Line));
    n->text = strdup(text);
    n->next = NULL;
    if (idx == 0) { n->next = head; head = n; }
    else {
        Line *p = head;
        for (int i = 0; i < idx - 1; i++) p = p->next;
        n->next = p->next;
        p->next = n;
    }
    count++;
}

void append_line(const char *text) {
    Line *n = malloc(sizeof(Line));
    n->text = strdup(text);
    n->next = NULL;
    if (!head) head = n;
    else {
        Line *p = head;
        while (p->next) p = p->next;
        p->next = n;
    }
    count++;
}

void delete_line(int idx) {
    if (idx < 0 || idx >= count) return;
    if (idx == 0) {
        Line *tmp = head;
        head = head->next;
        free(tmp->text);
        free(tmp);
    } else {
        Line *p = head;
        for (int i = 0; i < idx - 1; i++) p = p->next;
        Line *tmp = p->next;
        p->next = tmp->next;
        free(tmp->text);
        free(tmp);
    }
    count--;
}

void patch_line(int idx, const char *text) {
    if (idx < 0 || idx >= count) return;
    Line *p = head;
    for (int i = 0; i < idx; i++) p = p->next;
    free(p->text);
    p->text = strdup(text);
}

void print_lines() {
    for (Line *p = head; p; p = p->next) {
        for (char *c = p->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

void cleanup() {
    while (head) {
        Line *tmp = head->next;
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
        char cmd[32];
        int idx;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d", &idx) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) insert_line(idx, t + 1); }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *t = strchr(line, ' ');
            if (t) append_line(t + 1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(line, "%*s %d", &idx) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) patch_line(idx, t + 1); }
            }
        } else if (strcmp(cmd, "PRINT") == 0) print_lines();
    }
    cleanup();
    return 0;
}