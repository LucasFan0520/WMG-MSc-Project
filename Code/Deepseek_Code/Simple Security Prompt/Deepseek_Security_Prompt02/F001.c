/* F001.c */
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

void add_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (p) {
        free(p->text);
        p->text = strdup(text);
        return;
    }
    p = malloc(sizeof(Paragraph));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = head;
    head = p;
}

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    size_t newlen = strlen(p->text) + strlen(text) + 1;
    char *newtext = malloc(newlen);
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
    Paragraph *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free_paragraph(curr);
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

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char id[256];
    char text[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "NEW") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) != 1) continue;
            p += strlen(id);
            while (*p == ' ') p++;
            add_paragraph(id, p);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) != 1) continue;
            p += strlen(id);
            while (*p == ' ') p++;
            append_paragraph(id, p);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) != 1) continue;
            p += strlen(id);
            while (*p == ' ') p++;
            replace_paragraph(id, p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) == 1) delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", id) == 1) show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }

    while (head) {
        Paragraph *tmp = head;
        head = head->next;
        free_paragraph(tmp);
    }
    return 0;
}