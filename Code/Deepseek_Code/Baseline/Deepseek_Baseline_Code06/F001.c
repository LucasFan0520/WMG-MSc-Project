/* F001.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Paragraph {
    char *id;
    char *text;
    struct Paragraph *next;
} Paragraph;

Paragraph *head = NULL;
Paragraph *tail = NULL;

Paragraph* find_paragraph(const char *id) {
    Paragraph *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_paragraph(const char *id, const char *text) {
    if (find_paragraph(id)) return;
    Paragraph *p = malloc(sizeof(Paragraph));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    char *newtext = malloc(strlen(p->text) + strlen(text) + 1);
    strcpy(newtext, p->text);
    strcat(newtext, text);
    free(p->text);
    p->text = newtext;
}

void replace_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    free(p->text);
    p->text = strdup(text);
}

void delete_paragraph(const char *id) {
    Paragraph *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (p == tail) tail = prev;
            free(p->id);
            free(p->text);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
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
    Paragraph *p = head;
    while (p) {
        printf("%s\n", p->id);
        p = p->next;
    }
}

void free_all() {
    while (head) {
        Paragraph *tmp = head;
        head = head->next;
        free(tmp->id);
        free(tmp->text);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char id[256]; char text[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, id, text);
        if (n < 2) continue;
        if (strcmp(cmd, "NEW") == 0 && n == 3) {
            add_paragraph(id, text);
        } else if (strcmp(cmd, "APPEND") == 0 && n == 3) {
            append_paragraph(id, text);
        } else if (strcmp(cmd, "REPLACE") == 0 && n == 3) {
            replace_paragraph(id, text);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0 && n == 2) {
            show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_paragraphs();
        }
    }
    free_all();
    return 0;
}