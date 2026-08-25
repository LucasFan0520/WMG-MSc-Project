// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;
Line *tail = NULL;
int line_count = 0;

Line *get_line(int index) {
    if (index < 0 || index >= line_count) return NULL;
    Line *l = head;
    for (int i = 0; i < index; i++) l = l->next;
    return l;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    Line *new = malloc(sizeof(Line));
    new->text = malloc(strlen(text) + 1);
    strcpy(new->text, text);
    if (index == 0) {
        new->next = head;
        head = new;
        if (!tail) tail = new;
    } else {
        Line *prev = get_line(index - 1);
        new->next = prev->next;
        prev->next = new;
        if (!new->next) tail = new;
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
        if (!head) tail = NULL;
    } else {
        Line *prev = get_line(index - 1);
        to_del = prev->next;
        prev->next = to_del->next;
        if (!prev->next) tail = prev;
    }
    free(to_del->text);
    free(to_del);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line(index);
    if (!l) return;
    free(l->text);
    l->text = malloc(strlen(text) + 1);
    strcpy(l->text, text);
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &idx) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            insert_line(idx, p);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            append_line(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &idx) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            patch_line(idx, p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free_all();
    return 0;
}