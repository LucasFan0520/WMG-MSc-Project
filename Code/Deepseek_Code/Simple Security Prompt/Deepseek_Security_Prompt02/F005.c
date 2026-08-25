/* F005.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;
int line_count = 0;

Line *get_line(int index) {
    if (index < 0 || index >= line_count) return NULL;
    Line *l = head;
    for (int i = 0; i < index; i++) l = l->next;
    return l;
}

void insert_line(int index, const char *text) {
    if (index < 0 || index > line_count) return;
    Line *newl = malloc(sizeof(Line));
    newl->text = strdup(text);
    if (index == 0) {
        newl->next = head;
        head = newl;
    } else {
        Line *prev = get_line(index - 1);
        newl->next = prev->next;
        prev->next = newl;
    }
    line_count++;
}

void append_line(const char *text) {
    insert_line(line_count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= line_count) return;
    Line *to_delete;
    if (index == 0) {
        to_delete = head;
        head = head->next;
    } else {
        Line *prev = get_line(index - 1);
        to_delete = prev->next;
        prev->next = to_delete->next;
    }
    free(to_delete->text);
    free(to_delete);
    line_count--;
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
        for (char *c = l->text; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        l = l->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    int index;
    char text[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "INSERT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &index) != 1) continue;
            while (*p == ' ') p++;
            p += (index >= 0 ? 1 : 0); 
            while (*p == ' ') p++;
            insert_line(index, p);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            append_line(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &index) == 1) delete_line(index);
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%d", &index) != 1) continue;
            while (*p == ' ') p++;
            p += (index >= 0 ? 1 : 0);
            while (*p == ' ') p++;
            patch_line(index, p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }

    while (head) {
        Line *tmp = head;
        head = head->next;
        free(tmp->text);
        free(tmp);
    }
    return 0;
}