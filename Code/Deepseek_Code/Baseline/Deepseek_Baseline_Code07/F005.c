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
    Line *l = head;
    int i = 0;
    while (l) {
        if (i == index) return l;
        l = l->next;
        i++;
    }
    return NULL;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    Line *new = malloc(sizeof(Line));
    new->text = malloc(strlen(text)+1);
    strcpy(new->text, text);
    if (index == 0) {
        new->next = head;
        head = new;
        if (!tail) tail = new;
    } else if (index == line_count) {
        new->next = NULL;
        if (tail) tail->next = new;
        tail = new;
    } else {
        Line *prev = get_line(index-1);
        new->next = prev->next;
        prev->next = new;
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
        Line *prev = get_line(index-1);
        to_del = prev->next;
        prev->next = to_del->next;
        if (index == line_count-1) tail = prev;
    }
    free(to_del->text);
    free(to_del);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line(index);
    if (!l) return;
    free(l->text);
    l->text = malloc(strlen(text)+1);
    strcpy(l->text, text);
}

void print_lines(void) {
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_str = strtok(rest, " ");
            if (!idx_str) continue;
            int idx = atoi(idx_str);
            char *text = strtok(NULL, "");
            if (!text) text = "";
            insert_line(idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_line(rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_str = strtok(rest, " ");
            if (idx_str) delete_line(atoi(idx_str));
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_str = strtok(rest, " ");
            if (!idx_str) continue;
            int idx = atoi(idx_str);
            char *text = strtok(NULL, "");
            if (!text) text = "";
            patch_line(idx, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free_all();
    return 0;
}