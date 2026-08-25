// F001.c
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

Paragraph *find_paragraph(const char *id) {
    Paragraph *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_paragraph(const char *id, const char *text) {
    Paragraph *p = malloc(sizeof(Paragraph));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (tail) {
        tail->next = p;
        tail = p;
    } else {
        head = tail = p;
    }
}

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    char *newtext;
    if (p->text[0] == '\0') {
        newtext = strdup(text);
    } else {
        newtext = malloc(strlen(p->text) + 1 + strlen(text) + 1);
        sprintf(newtext, "%s %s", p->text, text);
    }
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
    Paragraph *prev = NULL;
    Paragraph *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (!p->next) tail = prev;
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
    for (char *s = p->text; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void list_paragraphs() {
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

void free_all() {
    Paragraph *p = head;
    while (p) {
        Paragraph *next = p->next;
        free(p->id);
        free(p->text);
        free(p);
        p = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256];
        char text[4096];
        if (sscanf(line, "%31s %255s", cmd, id) < 2) continue;
        const char *rest = line + strlen(cmd) + strlen(id) + 2;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0) {
            if (*rest) add_paragraph(id, rest);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (*rest) append_paragraph(id, rest);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            if (*rest) replace_paragraph(id, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    free_all();
    return 0;
}