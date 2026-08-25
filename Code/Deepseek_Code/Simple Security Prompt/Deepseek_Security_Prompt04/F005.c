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
int count = 0;

Line *get_line_at(int index) {
    if (index < 0 || index >= count) return NULL;
    Line *cur = head;
    for (int i = 0; i < index; i++) cur = cur->next;
    return cur;
}

void insert_line(int index, char *text) {
    if (index < 0 || index > count) return;
    Line *new = malloc(sizeof(Line));
    new->text = strdup(text);
    if (index == 0) {
        new->next = head;
        head = new;
        if (!tail) tail = new;
    } else if (index == count) {
        new->next = NULL;
        tail->next = new;
        tail = new;
    } else {
        Line *prev = get_line_at(index - 1);
        new->next = prev->next;
        prev->next = new;
    }
    count++;
}

void append_line(char *text) {
    insert_line(count, text);
}

void delete_line(int index) {
    if (index < 0 || index >= count) return;
    Line *to_del;
    if (index == 0) {
        to_del = head;
        head = head->next;
        if (!head) tail = NULL;
    } else {
        Line *prev = get_line_at(index - 1);
        to_del = prev->next;
        prev->next = to_del->next;
        if (to_del == tail) tail = prev;
    }
    free(to_del->text);
    free(to_del);
    count--;
}

void patch_line(int index, char *text) {
    Line *l = get_line_at(index);
    if (!l) return;
    free(l->text);
    l->text = strdup(text);
}

void print_all(void) {
    for (Line *l = head; l; l = l->next) {
        for (char *c = l->text; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "INSERT") == 0 && n == 3) {
            insert_line(atoi(arg1), arg2);
        } else if (strcmp(cmd, "APPEND") == 0 && n == 2) {
            // arg1 is the text (since text may contain spaces)
            // Actually APPEND text: arg1 is text
            append_line(arg1);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_line(atoi(arg1));
        } else if (strcmp(cmd, "PATCH") == 0 && n == 3) {
            patch_line(atoi(arg1), arg2);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_all();
        }
    }
    free(line);
    free_all();
    return 0;
}