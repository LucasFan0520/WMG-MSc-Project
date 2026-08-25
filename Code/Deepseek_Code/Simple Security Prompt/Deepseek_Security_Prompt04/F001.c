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

void add_paragraph(char *id, char *text) {
    Paragraph *p = malloc(sizeof(Paragraph));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (!tail) head = tail = p;
    else { tail->next = p; tail = p; }
}

void append_paragraph(char *id, char *text) {
    for (Paragraph *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            char *newtext = malloc(strlen(p->text) + strlen(text) + 1);
            strcpy(newtext, p->text);
            strcat(newtext, text);
            free(p->text);
            p->text = newtext;
            return;
        }
    }
}

void replace_paragraph(char *id, char *text) {
    for (Paragraph *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            free(p->text);
            p->text = strdup(text);
            return;
        }
    }
}

void delete_paragraph(char *id) {
    Paragraph *prev = NULL;
    for (Paragraph *p = head; p; p = p->next) {
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
    }
}

void show_paragraph(char *id) {
    for (Paragraph *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) {
            for (char *c = p->text; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            return;
        }
    }
    printf("NOT_FOUND\n");
}

void list_all(void) {
    for (Paragraph *p = head; p; p = p->next) {
        printf("%s\n", p->id);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], id[256], text[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, id, text);
        if (n < 1) continue;
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
            list_all();
        }
    }
    free(line);
    free_all();
    return 0;
}