// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct line_node {
    char *text;
    struct line_node *next;
} Line;

Line *head = NULL;
int line_count = 0;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Line *get_line(int index) {
    if (index < 0 || index >= line_count) return NULL;
    Line *l = head;
    for (int i = 0; i < index; i++) l = l->next;
    return l;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    Line *newl = malloc(sizeof(Line));
    if (!newl) { fprintf(stderr, "Memory error\n"); exit(1); }
    newl->text = strdup(text);
    if (!newl->text) { fprintf(stderr, "Memory error\n"); exit(1); }
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
        Line *prev = get_line(index - 1);
        to_del = prev->next;
        prev->next = to_del->next;
    }
    free(to_del->text);
    free(to_del);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line(index);
    if (!l) return;
    free(l->text);
    l->text = strdup(text);
    if (!l->text) { fprintf(stderr, "Memory error\n"); exit(1); }
}

void print_lines(void) {
    Line *l = head;
    while (l) {
        for (char *s = l->text; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
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
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "PRINT") == 0) { print_lines(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "INSERT") == 0 || strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "PATCH") == 0) {
            char *idx_str = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            int index = atoi(idx_str);
            rest++;
            while (isspace(*rest)) rest++;
            if (strcmp(cmd, "INSERT") == 0) {
                insert_line(index, rest);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_line(index);
            } else if (strcmp(cmd, "PATCH") == 0) {
                patch_line(index, rest);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_line(rest);
        }
    }
    free_all();
    return 0;
}