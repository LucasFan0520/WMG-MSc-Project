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

Paragraph *find_paragraph(const char *id) {
    Paragraph *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_new(const char *id, const char *text) {
    if (find_paragraph(id)) return;
    Paragraph *p = malloc(sizeof(Paragraph));
    if (!p) return;
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (!p->id || !p->text) {
        free(p->id); free(p->text); free(p);
        return;
    }
    if (!head) head = p;
    else {
        Paragraph *last = head;
        while (last->next) last = last->next;
        last->next = p;
    }
}

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    size_t newlen = strlen(p->text) + 1 + strlen(text) + 1;
    char *newtext = malloc(newlen);
    if (!newtext) return;
    if (strlen(p->text) > 0)
        snprintf(newtext, newlen, "%s %s", p->text, text);
    else
        snprintf(newtext, newlen, "%s", text);
    free(p->text);
    p->text = newtext;
}

void replace_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    char *newtext = strdup(text);
    if (!newtext) return;
    free(p->text);
    p->text = newtext;
}

void delete_paragraph(const char *id) {
    Paragraph *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
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
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void list_paragraphs(void) {
    Paragraph *p = head;
    while (p) {
        printf("%s\n", p->id);
        p = p->next;
    }
}

void free_all(void) {
    Paragraph *p = head;
    while (p) {
        Paragraph *next = p->next;
        free(p->id);
        free(p->text);
        free(p);
        p = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], id[256], text[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text) == 2)
                add_new(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text) == 2)
                append_paragraph(id, text);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", id, text) == 2)
                replace_paragraph(id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (sscanf(line, "%*s %255s", id) == 1)
                show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    free(line);
    free_all();
    return 0;
}