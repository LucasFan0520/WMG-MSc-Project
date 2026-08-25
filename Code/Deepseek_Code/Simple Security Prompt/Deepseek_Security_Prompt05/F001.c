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

Paragraph *find_paragraph(const char *id) {
    Paragraph *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void free_paragraph(Paragraph *p) {
    free(p->id);
    free(p->text);
    free(p);
}

void new_paragraph(const char *id, const char *text) {
    if (find_paragraph(id)) return;
    Paragraph *p = malloc(sizeof(Paragraph));
    if (!p) return;
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = head;
    head = p;
}

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    size_t newlen = strlen(p->text) + strlen(text) + 2;
    char *newtext = malloc(newlen);
    if (!newtext) return;
    sprintf(newtext, "%s %s", p->text, text);
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
            free_paragraph(p);
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
    char *text = strdup(p->text);
    for (char *c = text; *c; c++) if (*c == ' ') *c = '_';
    printf("%s\n", text);
    free(text);
}

void list_all(void) {
    Paragraph *p = head;
    while (p) {
        printf("%s\n", p->id);
        p = p->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], id[256], text[2048];
        int n = sscanf(line, "%15s %255s", cmd, id);
        if (n < 2) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *t = line + strlen(cmd) + strlen(id) + 2;
            while (*t == ' ') t++;
            new_paragraph(id, t);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *t = line + strlen(cmd) + strlen(id) + 2;
            while (*t == ' ') t++;
            append_paragraph(id, t);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *t = line + strlen(cmd) + strlen(id) + 2;
            while (*t == ' ') t++;
            replace_paragraph(id, t);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_all();
        }
    }
    while (head) {
        Paragraph *tmp = head;
        head = head->next;
        free_paragraph(tmp);
    }
    return 0;
}