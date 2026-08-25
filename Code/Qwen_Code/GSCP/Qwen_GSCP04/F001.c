// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Paragraph {
    char *id;
    char *text;
    struct Paragraph *next;
} Paragraph;

Paragraph *head = NULL;
Paragraph *tail = NULL;

Paragraph *find_paragraph(const char *id) {
    Paragraph *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (p) {
        free(p->text);
        p->text = strdup(text);
    } else {
        p = malloc(sizeof(Paragraph));
        p->id = strdup(id);
        p->text = strdup(text);
        p->next = NULL;
        if (tail) tail->next = p;
        else head = p;
        tail = p;
    }
}

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (p) {
        size_t old_len = strlen(p->text);
        size_t add_len = strlen(text);
        p->text = realloc(p->text, old_len + add_len + 2);
        strcat(p->text, " ");
        strcat(p->text, text);
    } else {
        add_paragraph(id, text);
    }
}

void delete_paragraph(const char *id) {
    Paragraph *curr = head, *prev = NULL;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (tail == curr) tail = prev;
            free(curr->id);
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_paragraph(const char *id) {
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

void list_paragraphs() {
    Paragraph *curr = head;
    while (curr) {
        printf("%s\n", curr->id);
        curr = curr->next;
    }
}

void free_all() {
    Paragraph *curr = head;
    while (curr) {
        Paragraph *next = curr->next;
        free(curr->id);
        free(curr->text);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], id[256], text[4096] = {0};
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, id, text);
        
        if (strcmp(cmd, "NEW") == 0 && n >= 2) add_paragraph(id, text);
        else if (strcmp(cmd, "APPEND") == 0 && n >= 2) append_paragraph(id, text);
        else if (strcmp(cmd, "REPLACE") == 0 && n >= 2) add_paragraph(id, text);
        else if (strcmp(cmd, "DELETE") == 0 && n >= 1) delete_paragraph(id);
        else if (strcmp(cmd, "SHOW") == 0 && n >= 1) show_paragraph(id);
        else if (strcmp(cmd, "LIST") == 0) list_paragraphs();
    }
    free_all();
    return 0;
}