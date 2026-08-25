// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_TEXT 4096

typedef struct Line {
    char text[MAX_TEXT];
    struct Line *next;
} Line;

Line *head = NULL;
int line_count = 0;

Line *get_line(int index) {
    if (index < 0 || index >= line_count) return NULL;
    Line *l = head;
    for (int i = 0; i < index; i++) l = l->next;
    return l;
}

void free_all() {
    Line *l = head;
    while (l) {
        Line *tmp = l;
        l = l->next;
        free(tmp);
    }
    head = NULL;
    line_count = 0;
}

void handle_insert(char *line) {
    int index;
    char text[MAX_TEXT];
    if (sscanf(line, "INSERT %d %4095[^\n]", &index, text) != 2) return;
    if (index < 0 || index > line_count) return;
    Line *newl = malloc(sizeof(Line));
    if (!newl) return;
    strcpy(newl->text, text);
    if (index == 0) {
        newl->next = head;
        head = newl;
    } else {
        Line *prev = get_line(index - 1);
        newl->next = prev->next;
        prev->next = newl;
    }
    line_count++;
}

void handle_append(char *line) {
    char text[MAX_TEXT];
    if (sscanf(line, "APPEND %4095[^\n]", text) != 1) return;
    Line *newl = malloc(sizeof(Line));
    if (!newl) return;
    strcpy(newl->text, text);
    newl->next = NULL;
    if (!head) {
        head = newl;
    } else {
        Line *last = head;
        while (last->next) last = last->next;
        last->next = newl;
    }
    line_count++;
}

void handle_delete(char *line) {
    int index;
    if (sscanf(line, "DELETE %d", &index) != 1) return;
    if (index < 0 || index >= line_count) return;
    Line *to_del;
    if (index == 0) {
        to_del = head;
        head = head->next;
    } else {
        Line *prev = get_line(index - 1);
        to_del = prev->next;
        prev->next = to_del->next;
    }
    free(to_del);
    line_count--;
}

void handle_patch(char *line) {
    int index;
    char text[MAX_TEXT];
    if (sscanf(line, "PATCH %d %4095[^\n]", &index, text) != 2) return;
    Line *l = get_line(index);
    if (!l) return;
    strcpy(l->text, text);
}

void handle_print() {
    Line *l = head;
    while (l) {
        for (char *c = l->text; *c; c++) {
            if (*c == ' ') putchar('_');
            else putchar(*c);
        }
        putchar('\n');
        l = l->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "INSERT ", 7) == 0) handle_insert(line);
        else if (strncmp(line, "APPEND ", 7) == 0) handle_append(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "PATCH ", 6) == 0) handle_patch(line);
        else if (strcmp(line, "PRINT") == 0) handle_print();
    }
    free_all();
    return 0;
}