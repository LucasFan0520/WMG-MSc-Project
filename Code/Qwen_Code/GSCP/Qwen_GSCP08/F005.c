// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Line {
    char *text;
    struct Line *next;
} Line;

Line *head = NULL;
int size = 0;

void insert_line(int index, char *text) {
    if (index < 0 || index > size) return;
    Line *node = malloc(sizeof(Line));
    node->text = strdup(text);
    node->next = NULL;
    if (index == 0) {
        node->next = head;
        head = node;
    } else {
        Line *curr = head;
        for (int i = 0; i < index - 1; i++) curr = curr->next;
        node->next = curr->next;
        curr->next = node;
    }
    size++;
}

void append_line(char *text) {
    insert_line(size, text);
}

void delete_line(int index) {
    if (index < 0 || index >= size) return;
    Line *prev = NULL;
    Line *curr = head;
    for (int i = 0; i < index; i++) {
        prev = curr;
        curr = curr->next;
    }
    if (prev) prev->next = curr->next;
    else head = curr->next;
    free(curr->text);
    free(curr);
    size--;
}

void patch_line(int index, char *text) {
    if (index < 0 || index >= size) return;
    Line *curr = head;
    for (int i = 0; i < index; i++) curr = curr->next;
    free(curr->text);
    curr->text = strdup(text);
}

void print_lines() {
    for (Line *l = head; l; l = l->next) {
        for (char *c = l->text; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
}

int main() {
    char cmd[20], text[1000];
    int index;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d", &index);
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            insert_line(index, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            append_line(text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%d", &index);
            delete_line(index);
        } else if (strcmp(cmd, "PATCH") == 0) {
            scanf("%d", &index);
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            patch_line(index, text);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_lines();
        }
    }
    return 0;
}