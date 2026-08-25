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

Line *get_line(int index) {
    if (index < 0 || index >= count) return NULL;
    Line *p = head;
    for (int i = 0; i < index; i++) p = p->next;
    return p;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > count) return;
    Line *newl = malloc(sizeof(Line));
    newl->text = malloc(strlen(text) + 1);
    strcpy(newl->text, text);
    if (index == 0) {
        newl->next = head;
        head = newl;
    } else {
        Line *prev = get_line(index - 1);
        newl->next = prev->next;
        prev->next = newl;
    }
    count++;
}

void append_line(const char *text) {
    Line *newl = malloc(sizeof(Line));
    newl->text = malloc(strlen(text) + 1);
    strcpy(newl->text, text);
    newl->next = NULL;
    if (!head) head = newl;
    else {
        Line *p = head;
        while (p->next) p = p->next;
        p->next = newl;
    }
    count++;
}

void delete_line(int index) {
    if (index < 0 || index >= count) return;
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
    count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line(index);
    if (!l) return;
    free(l->text);
    l->text = malloc(strlen(text) + 1);
    strcpy(l->text, text);
}

void print_lines(void) {
    Line *p = head;
    while (p) {
        for (char *s = p->text; *s; s++) {
            if (*s == ' ') putchar('_');
            else putchar(*s);
        }
        putchar('\n');
        p = p->next;
    }
}

void free_all(void) {
    Line *p = head;
    while (p) {
        Line *tmp = p->next;
        free(p->text);
        free(p);
        p = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        int idx;
        char text[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(rest, "%d", &idx) == 1) {
                const char *t = rest;
                while (*t && *t != ' ') t++;
                while (*t == ' ') t++;
                if (*t) insert_line(idx, t);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (*rest) append_line(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(rest, "%d", &idx) == 1) {
                const char *t = rest;
                while (*t && *t != ' ') t++;
                while (*t == ' ') t++;
                if (*t) patch_line(idx, t);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free_all();
    return 0;
}