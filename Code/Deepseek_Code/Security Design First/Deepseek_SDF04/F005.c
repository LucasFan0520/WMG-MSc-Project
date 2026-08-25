// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;

int count_lines(void) {
    int c = 0;
    Line *l = head;
    while (l) { c++; l = l->next; }
    return c;
}

Line *get_line_at(int index) {
    Line *l = head;
    while (l && index--) l = l->next;
    return l;
}

void insert_line(int index, const char *text) {
    int n = count_lines();
    if (index < 0 || index > n) return;
    Line *new = malloc(sizeof(Line));
    if (!new) return;
    new->text = strdup(text);
    new->next = NULL;
    if (!new->text) { free(new); return; }
    if (index == 0) {
        new->next = head;
        head = new;
    } else {
        Line *prev = get_line_at(index - 1);
        if (!prev) { free(new->text); free(new); return; }
        new->next = prev->next;
        prev->next = new;
    }
}

void append_line(const char *text) {
    insert_line(count_lines(), text);
}

void delete_line(int index) {
    int n = count_lines();
    if (index < 0 || index >= n) return;
    Line *l = head, *prev = NULL;
    while (l && index--) { prev = l; l = l->next; }
    if (!l) return;
    if (prev) prev->next = l->next;
    else head = l->next;
    free(l->text);
    free(l);
}

void patch_line(int index, const char *text) {
    Line *l = get_line_at(index);
    if (!l) return;
    char *new = strdup(text);
    if (!new) return;
    free(l->text);
    l->text = new;
}

void print_lines(void) {
    Line *l = head;
    while (l) {
        for (char *c = l->text; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        l = l->next;
    }
}

void free_all(void) {
    Line *l = head;
    while (l) {
        Line *next = l->next;
        free(l->text);
        free(l);
        l = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], text[4096];
        int idx;
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %4095[^\n]", &idx, text) == 2)
                insert_line(idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", text) == 1)
                append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %d", &idx) == 1)
                delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(line, "%*s %d %4095[^\n]", &idx, text) == 2)
                patch_line(idx, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free(line);
    free_all();
    return 0;
}