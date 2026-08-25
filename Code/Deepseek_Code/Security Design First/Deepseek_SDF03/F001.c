// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_ID 64
#define MAX_TEXT 4096

typedef struct Paragraph {
    char id[MAX_ID];
    char text[MAX_TEXT];
    struct Paragraph *next;
} Paragraph;

Paragraph *head = NULL;

Paragraph *find_paragraph(const char *id) {
    Paragraph *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void free_all() {
    Paragraph *p = head;
    while (p) {
        Paragraph *tmp = p;
        p = p->next;
        free(tmp);
    }
    head = NULL;
}

void handle_new(char *line) {
    char id[MAX_ID];
    char text[MAX_TEXT];
    int n = sscanf(line, "NEW %63s %4095[^\n]", id, text);
    if (n < 2) return;
    if (find_paragraph(id)) return;
    Paragraph *p = malloc(sizeof(Paragraph));
    if (!p) return;
    strcpy(p->id, id);
    strcpy(p->text, text);
    p->next = head;
    head = p;
}

void handle_append(char *line) {
    char id[MAX_ID];
    char text[MAX_TEXT];
    int n = sscanf(line, "APPEND %63s %4095[^\n]", id, text);
    if (n < 2) return;
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    size_t cur_len = strlen(p->text);
    size_t add_len = strlen(text);
    if (cur_len + add_len + 1 > MAX_TEXT) return;
    strcat(p->text, text);
}

void handle_replace(char *line) {
    char id[MAX_ID];
    char text[MAX_TEXT];
    int n = sscanf(line, "REPLACE %63s %4095[^\n]", id, text);
    if (n < 2) return;
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    strcpy(p->text, text);
}

void handle_delete(char *line) {
    char id[MAX_ID];
    if (sscanf(line, "DELETE %63s", id) != 1) return;
    Paragraph *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void handle_show(char *line) {
    char id[MAX_ID];
    if (sscanf(line, "SHOW %63s", id) != 1) return;
    Paragraph *p = find_paragraph(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = p->text; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void handle_list() {
    Paragraph *p = head;
    int first = 1;
    while (p) {
        if (!first) putchar(' ');
        printf("%s", p->id);
        first = 0;
        p = p->next;
    }
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "NEW ", 4) == 0) handle_new(line);
        else if (strncmp(line, "APPEND ", 7) == 0) handle_append(line);
        else if (strncmp(line, "REPLACE ", 8) == 0) handle_replace(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "SHOW ", 5) == 0) handle_show(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}