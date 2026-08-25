// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct line {
    char *text;
    struct line *next;
} Line;

Line *head = NULL;
Line *tail = NULL;
int count = 0;

Line *get_line(int index) {
    if (index < 0) return NULL;
    Line *l = head;
    for (int i = 0; i < index && l; i++) l = l->next;
    return l;
}

void insert_line(int index, const char *text) {
    if (index < 0) return;
    Line *new = malloc(sizeof(Line));
    new->text = strdup(text);
    if (index == 0) {
        new->next = head;
        head = new;
        if (!tail) tail = new;
    } else {
        Line *prev = get_line(index - 1);
        if (!prev) { free(new->text); free(new); return; }
        new->next = prev->next;
        prev->next = new;
        if (!new->next) tail = new;
    }
    count++;
}

void append_line(const char *text) {
    Line *new = malloc(sizeof(Line));
    new->text = strdup(text);
    new->next = NULL;
    if (tail) tail->next = new;
    else head = new;
    tail = new;
    count++;
}

void delete_line(int index) {
    if (index < 0) return;
    if (index == 0) {
        Line *tmp = head;
        if (tmp) {
            head = tmp->next;
            if (!head) tail = NULL;
            free(tmp->text);
            free(tmp);
            count--;
        }
        return;
    }
    Line *prev = get_line(index - 1);
    if (!prev || !prev->next) return;
    Line *tmp = prev->next;
    prev->next = tmp->next;
    if (!prev->next) tail = prev;
    free(tmp->text);
    free(tmp);
    count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line(index);
    if (!l) return;
    free(l->text);
    l->text = strdup(text);
}

void print_lines(void) {
    Line *l = head;
    while (l) {
        for (char *c = l->text; *c; c++)
            putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        l = l->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        int index;
        char text[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %[^\n]", &index, text) == 2)
                insert_line(index, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %[^\n]", text) == 1)
                append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %d", &index) == 1)
                delete_line(index);
        } else if (strcmp(cmd, "PATCH") == 0) {
            if (sscanf(line, "%*s %d %[^\n]", &index, text) == 2)
                patch_line(index, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    return 0;
}