// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;
Line *tail = NULL;
int line_count = 0;

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

Line *get_line_at(int index) {
    Line *cur = head;
    for (int i = 0; i < index && cur; i++) cur = cur->next;
    return cur;
}

void insert_line(int index, const char *text) {
    if (index < 0) return;
    Line *new_line = malloc(sizeof(Line));
    if (!new_line) return;
    new_line->text = safe_strdup(text);
    new_line->next = NULL;

    if (index == 0) {
        new_line->next = head;
        head = new_line;
        if (!tail) tail = new_line;
    } else {
        Line *prev = get_line_at(index - 1);
        if (!prev) {
            free(new_line->text);
            free(new_line);
            return;
        }
        new_line->next = prev->next;
        prev->next = new_line;
        if (new_line->next == NULL) tail = new_line;
    }
    line_count++;
}

void append_line(const char *text) {
    Line *new_line = malloc(sizeof(Line));
    if (!new_line) return;
    new_line->text = safe_strdup(text);
    new_line->next = NULL;
    if (tail) {
        tail->next = new_line;
        tail = new_line;
    } else {
        head = tail = new_line;
    }
    line_count++;
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    if (index == 0) {
        Line *to_free = head;
        head = head->next;
        if (!head) tail = NULL;
        free(to_free->text);
        free(to_free);
    } else {
        Line *prev = get_line_at(index - 1);
        if (!prev) return;
        Line *to_free = prev->next;
        if (!to_free) return;
        prev->next = to_free->next;
        if (prev->next == NULL) tail = prev;
        free(to_free->text);
        free(to_free);
    }
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *line = get_line_at(index);
    if (line) {
        free(line->text);
        line->text = safe_strdup(text);
    }
}

void print_lines(void) {
    Line *cur = head;
    while (cur) {
        for (const char *s = cur->text; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        cur = cur->next;
    }
}

void free_all(void) {
    Line *cur = head;
    while (cur) {
        Line *next = cur->next;
        free(cur->text);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512];
        int n = sscanf(line, "%31s %511s", cmd, arg1);
        if (n < 1) continue;

        if (strcmp(cmd, "INSERT") == 0 && n >= 2) {
            int index = atoi(arg1);
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            insert_line(index, p);
        } else if (strcmp(cmd, "APPEND") == 0 && n >= 2) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            append_line(p);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int index = atoi(arg1);
            delete_line(index);
        } else if (strcmp(cmd, "PATCH") == 0 && n >= 2) {
            int index = atoi(arg1);
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            patch_line(index, p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free_all();
    return 0;
}