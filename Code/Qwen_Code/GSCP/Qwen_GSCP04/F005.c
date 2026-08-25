// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;
int line_count = 0;

Line *get_line_at(int index) {
    if (index < 0 || index >= line_count) return NULL;
    Line *curr = head;
    for (int i = 0; i < index; i++) curr = curr->next;
    return curr;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    Line *new_line = malloc(sizeof(Line));
    new_line->text = strdup(text);
    if (index == 0) {
        new_line->next = head;
        head = new_line;
    } else {
        Line *prev = get_line_at(index - 1);
        new_line->next = prev->next;
        prev->next = new_line;
    }
    line_count++;
}

void append_line(const char *text) {
    insert_line(line_count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    Line *to_del;
    if (index == 0) {
        to_del = head;
        head = head->next;
    } else {
        Line *prev = get_line_at(index - 1);
        to_del = prev->next;
        prev->next = to_del->next;
    }
    free(to_del->text);
    free(to_del);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line_at(index);
    if (l) {
        free(l->text);
        l->text = strdup(text);
    }
}

void print_lines() {
    Line *curr = head;
    while (curr) {
        for (char *c = curr->text; *c; c++) {
            if (*c == ' ') putchar('_');
            else putchar(*c);
        }
        putchar('\n');
        curr = curr->next;
    }
}

void free_all() {
    Line *curr = head;
    while (curr) {
        Line *next = curr->next;
        free(curr->text);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], text[4096] = {0};
        int index;
        int n = sscanf(line, "%31s %d %4095[^\n]", cmd, &index, text);
        
        if (strcmp(cmd, "INSERT") == 0 && n >= 2) insert_line(index, text);
        else if (strcmp(cmd, "APPEND") == 0) {
            char *t = strchr(line, ' ');
            if (t) append_line(t + 1);
        }
        else if (strcmp(cmd, "DELETE") == 0 && n >= 2) delete_line(index);
        else if (strcmp(cmd, "PATCH") == 0 && n >= 2) patch_line(index, text);
        else if (strcmp(cmd, "PRINT") == 0) print_lines();
    }
    free_all();
    return 0;
}