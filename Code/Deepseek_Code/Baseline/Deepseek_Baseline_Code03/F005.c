// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct line {
    char *text;
    struct line *next;
};

struct line *head = NULL;
struct line *tail = NULL;
int line_count = 0;

void insert_line(int index, char *text) {
    if (index < 0 || index > line_count) return;
    struct line *newl = malloc(sizeof(struct line));
    newl->text = malloc(strlen(text) + 1);
    strcpy(newl->text, text);
    if (index == 0) {
        newl->next = head;
        head = newl;
        if (!tail) tail = newl;
    } else {
        struct line *prev = head;
        for (int i = 0; i < index - 1; i++) prev = prev->next;
        newl->next = prev->next;
        prev->next = newl;
        if (newl->next == NULL) tail = newl;
    }
    line_count++;
}

void append_line(char *text) {
    insert_line(line_count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    struct line *to_del;
    if (index == 0) {
        to_del = head;
        head = head->next;
        if (!head) tail = NULL;
    } else {
        struct line *prev = head;
        for (int i = 0; i < index - 1; i++) prev = prev->next;
        to_del = prev->next;
        prev->next = to_del->next;
        if (to_del == tail) tail = prev;
    }
    free(to_del->text);
    free(to_del);
    line_count--;
}

void patch_line(int index, char *text) {
    if (index < 0 || index >= line_count) return;
    struct line *l = head;
    for (int i = 0; i < index; i++) l = l->next;
    free(l->text);
    l->text = malloc(strlen(text) + 1);
    strcpy(l->text, text);
}

void print_lines() {
    struct line *l = head;
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        int index;
        char text[4096];
        int n = sscanf(line, "%31s %d %4095[^\n]", cmd, &index, text);
        if (strcmp(cmd, "INSERT") == 0 && n >= 3) {
            insert_line(index, text);
        } else if (strcmp(cmd, "APPEND") == 0 && n >= 2) {
            char *rest = line + strlen(cmd) + 1;
            append_line(rest);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_line(index);
        } else if (strcmp(cmd, "PATCH") == 0 && n >= 3) {
            patch_line(index, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    return 0;
}