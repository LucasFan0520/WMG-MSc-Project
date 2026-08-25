/* F005.c */
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
    Line *l = head;
    int i = 0;
    while (l && i < index) {
        l = l->next;
        i++;
    }
    return l;
}

void insert_line(int index, const char *text) {
    if (index < 0) return;
    Line *newl = malloc(sizeof(Line));
    newl->text = strdup(text);
    if (index == 0) {
        newl->next = head;
        head = newl;
    } else {
        Line *prev = get_line_at(index - 1);
        if (!prev) { free(newl->text); free(newl); return; }
        newl->next = prev->next;
        prev->next = newl;
    }
    line_count++;
}

void append_line(const char *text) {
    Line *newl = malloc(sizeof(Line));
    newl->text = strdup(text);
    newl->next = NULL;
    if (!head) head = newl;
    else {
        Line *l = head;
        while (l->next) l = l->next;
        l->next = newl;
    }
    line_count++;
}

void delete_line(int index) {
    if (index < 0) return;
    if (index == 0) {
        Line *tofree = head;
        if (!tofree) return;
        head = tofree->next;
        free(tofree->text);
        free(tofree);
        line_count--;
        return;
    }
    Line *prev = get_line_at(index - 1);
    if (!prev || !prev->next) return;
    Line *tofree = prev->next;
    prev->next = tofree->next;
    free(tofree->text);
    free(tofree);
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char *p = line + 7;
            if (sscanf(p, "%d", &idx) != 1) continue;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            p++;
            while (*p == ' ') p++;
            insert_line(idx, p);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            append_line(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            char *p = line + 7;
            if (sscanf(p, "%d", &idx) == 1) delete_line(idx);
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            char *p = line + 6;
            if (sscanf(p, "%d", &idx) != 1) continue;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            p++;
            while (*p == ' ') p++;
            patch_line(idx, p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free_all();
    free(line);
    return 0;
}