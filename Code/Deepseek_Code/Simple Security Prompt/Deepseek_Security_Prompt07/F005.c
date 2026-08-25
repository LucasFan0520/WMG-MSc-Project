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
int count = 0;

Line *get_line(int index) {
    if (index < 0 || index >= count) return NULL;
    Line *l = head;
    for (int i = 0; i < index; i++) l = l->next;
    return l;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > count) return;
    Line *new = malloc(sizeof(Line));
    new->text = strdup(text);
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
    count++;
}

void append_line(const char *text) {
    insert_line(count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= count) return;
    Line *to_delete;
    if (index == 0) {
        to_delete = head;
        head = head->next;
        if (!head) tail = NULL;
    } else {
        Line *prev = get_line(index - 1);
        to_delete = prev->next;
        prev->next = to_delete->next;
        if (!prev->next) tail = prev;
    }
    free(to_delete->text);
    free(to_delete);
    count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line(index);
    if (!l) return;
    free(l->text);
    l->text = strdup(text);
}

void print_lines() {
    Line *l = head;
    while (l) {
        for (char *s = l->text; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        l = l->next;
    }
}

void free_all() {
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char text[4096];
            if (sscanf(rest, "%d %[^\n]", &idx, text) == 2) {
                insert_line(idx, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_line(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            if (sscanf(rest, "%d", &idx) == 1) {
                delete_line(idx);
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            char text[4096];
            if (sscanf(rest, "%d %[^\n]", &idx, text) == 2) {
                patch_line(idx, text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free_all();
    return 0;
}