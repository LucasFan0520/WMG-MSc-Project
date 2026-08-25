/* F005.c */
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

Line *get_line_at(int index) {
    if (index < 0 || index >= line_count) return NULL;
    Line *l = head;
    for (int i = 0; i < index; i++) l = l->next;
    return l;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    Line *new_l = malloc(sizeof(Line));
    if (!new_l) return;
    new_l->text = strdup(text);
    new_l->next = NULL;
    if (index == 0) {
        new_l->next = head;
        head = new_l;
        if (!tail) tail = new_l;
    } else if (index == line_count) {
        if (tail) tail->next = new_l;
        else head = new_l;
        tail = new_l;
    } else {
        Line *prev = get_line_at(index - 1);
        if (prev) {
            new_l->next = prev->next;
            prev->next = new_l;
        } else {
            free(new_l->text);
            free(new_l);
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
    Line *prev = NULL, *curr = head;
    for (int i = 0; i < index; i++) {
        prev = curr;
        curr = curr->next;
    }
    if (prev) prev->next = curr->next;
    else head = curr->next;
    if (curr == tail) tail = prev;
    free(curr->text);
    free(curr);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line_at(index);
    if (!l) return;
    free(l->text);
    l->text = strdup(text);
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
        Line *tmp = l;
        l = l->next;
        free(tmp->text);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &idx) == 1) {
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
                if (*p) insert_line(idx, p);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (*p) append_line(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            if (sscanf(line + 7, "%d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &idx) == 1) {
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
                if (*p) patch_line(idx, p);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free_all();
    return 0;
}