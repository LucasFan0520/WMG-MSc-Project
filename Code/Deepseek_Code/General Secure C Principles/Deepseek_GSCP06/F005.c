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
    if (index < 0) return NULL;
    Line *cur = head;
    int i = 0;
    while (cur) {
        if (i == index) return cur;
        cur = cur->next;
        i++;
    }
    return NULL;
}

void insert_line(int index, const char *text) {
    if (index < 0) return;
    if (index > line_count) index = line_count;
    Line *new = malloc(sizeof(Line));
    new->text = strdup(text);
    if (index == 0) {
        new->next = head;
        head = new;
    } else {
        Line *prev = get_line_at(index - 1);
        if (prev) {
            new->next = prev->next;
            prev->next = new;
        } else {
            // should not happen
            free(new->text);
            free(new);
            return;
        }
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
        if (!prev) return;
        to_del = prev->next;
        prev->next = to_del->next;
    }
    free(to_del->text);
    free(to_del);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line_at(index);
    if (!l) return;
    free(l->text);
    l->text = strdup(text);
}

void print_lines(void) {
    Line *cur = head;
    while (cur) {
        for (char *c = cur->text; *c; c++) {
            if (*c == ' ') putchar('_');
            else putchar(*c);
        }
        putchar('\n');
        cur = cur->next;
    }
}

void cleanup_lines(void) {
    Line *cur = head;
    while (cur) {
        Line *tmp = cur;
        cur = cur->next;
        free(tmp->text);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, arg1, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "INSERT") == 0 && n >= 3) {
            int idx = atoi(arg1);
            char *text = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            insert_line(idx, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = line + strlen(cmd) + 1;
            append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx = atoi(arg1);
            delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0 && n >= 3) {
            int idx = atoi(arg1);
            char *text = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            patch_line(idx, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    cleanup_lines();
    return 0;
}