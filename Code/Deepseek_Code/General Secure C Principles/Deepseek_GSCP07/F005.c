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

Line* get_line(int index) {
    Line *l = head;
    for (int i = 0; l && i < index; i++) l = l->next;
    return l;
}

void free_line(Line *l) {
    free(l->text);
    free(l);
}

void insert_line(int index, const char *text) {
    if (index < 0) return;
    Line *new = malloc(sizeof(Line));
    new->text = strdup(text);
    if (index == 0 || !head) {
        new->next = head;
        head = new;
        line_count++;
        return;
    }
    Line *prev = get_line(index - 1);
    if (!prev) {
        free(new->text);
        free(new);
        return;
    }
    new->next = prev->next;
    prev->next = new;
    line_count++;
}

void append_line(const char *text) {
    Line *new = malloc(sizeof(Line));
    new->text = strdup(text);
    new->next = NULL;
    if (!head) head = new;
    else {
        Line *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
    line_count++;
}

void delete_line(int index) {
    if (index < 0 || !head) return;
    if (index == 0) {
        Line *tmp = head;
        head = head->next;
        free_line(tmp);
        line_count--;
        return;
    }
    Line *prev = get_line(index - 1);
    if (!prev || !prev->next) return;
    Line *tmp = prev->next;
    prev->next = tmp->next;
    free_line(tmp);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line(index);
    if (!l) return;
    free(l->text);
    l->text = strdup(text);
}

void print_all() {
    Line *l = head;
    while (l) {
        for (char *s = l->text; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
        l = l->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "INSERT") == 0) {
            char *idxstr = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (idxstr && text) {
                int idx = atoi(idxstr);
                insert_line(idx, text);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = strtok(NULL, "");
            if (text) append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idxstr = strtok(NULL, " ");
            if (idxstr) {
                int idx = atoi(idxstr);
                delete_line(idx);
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idxstr = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (idxstr && text) {
                int idx = atoi(idxstr);
                patch_line(idx, text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    while (head) {
        Line *tmp = head;
        head = head->next;
        free_line(tmp);
    }
    return 0;
}