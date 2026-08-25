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

Line *get_line(int index) {
    if (index < 0) return NULL;
    Line *l = head;
    for (int i = 0; i < index; i++) {
        if (!l) return NULL;
        l = l->next;
    }
    return l;
}

void insert_line(int index, const char *text) {
    if (index < 0) return;
    Line *new = malloc(sizeof(Line));
    new->text = malloc(strlen(text) + 1);
    strcpy(new->text, text);
    if (index == 0) {
        new->next = head;
        head = new;
    } else {
        Line *prev = get_line(index - 1);
        if (!prev) { free(new->text); free(new); return; }
        new->next = prev->next;
        prev->next = new;
    }
    line_count++;
}

void append_line(const char *text) {
    Line *new = malloc(sizeof(Line));
    new->text = malloc(strlen(text) + 1);
    strcpy(new->text, text);
    new->next = NULL;
    if (!head) {
        head = new;
    } else {
        Line *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
    line_count++;
}

void delete_line(int index) {
    if (index < 0) return;
    Line *prev = NULL, *curr = head;
    for (int i = 0; i < index; i++) {
        if (!curr) return;
        prev = curr;
        curr = curr->next;
    }
    if (!curr) return;
    if (prev) prev->next = curr->next;
    else head = curr->next;
    free(curr->text);
    free(curr);
    line_count--;
}

void patch_line(int index, const char *text) {
    Line *l = get_line(index);
    if (!l) return;
    free(l->text);
    l->text = malloc(strlen(text) + 1);
    strcpy(l->text, text);
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (idx && text) insert_line(atoi(idx), text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = strtok(NULL, "");
            if (text) append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx = strtok(NULL, " ");
            if (idx) delete_line(atoi(idx));
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (idx && text) patch_line(atoi(idx), text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    free(line);
    free_all();
    return 0;
}