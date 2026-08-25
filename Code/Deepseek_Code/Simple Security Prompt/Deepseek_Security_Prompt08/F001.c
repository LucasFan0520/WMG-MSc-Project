// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Paragraph {
    char *id;
    char *text;
    struct Paragraph *next;
} Paragraph;

Paragraph *head = NULL, *tail = NULL;

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
    p->id = malloc(strlen(id) + 1);
    strcpy(p->id, id);
    p->text = malloc(strlen(text) + 1);
    strcpy(p->text, text);
    p->next = NULL;
    if (tail) {
        tail->next = p;
        tail = p;
    } else {
        head = tail = p;
    }
}

void append_text(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    char *new_text = malloc(strlen(p->text) + strlen(text) + 1);
    strcpy(new_text, p->text);
    strcat(new_text, text);
    free(p->text);
    p->text = new_text;
}

void replace_text(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    free(p->text);
    p->text = malloc(strlen(text) + 1);
    strcpy(p->text, text);
}

void delete_paragraph(const char *id) {
    Paragraph *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *id = strtok(NULL, " ");
        if (strcmp(cmd, "NEW") == 0) {
            char *text = strtok(NULL, "");
            if (id && text) add_paragraph(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = strtok(NULL, "");
            if (id && text) append_text(id, text);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *text = strtok(NULL, "");
            if (id && text) replace_text(id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (id) delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (id) show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    free(line);
    free_all();
    return 0;
}