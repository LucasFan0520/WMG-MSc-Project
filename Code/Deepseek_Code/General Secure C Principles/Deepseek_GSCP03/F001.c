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

Paragraph *find_paragraph(const char *id) {
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
    if (!p) return;
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
}

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) {
        add_paragraph(id, text);
        return;
    }
    char *new_text = malloc(strlen(p->text) + strlen(text) + 2);
    if (!new_text) return;
    sprintf(new_text, "%s %s", p->text, text);
    free(p->text);
    p->text = new_text;
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
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void list_paragraphs(void) {
    Paragraph *p = head;
    int first = 1;
    while (p) {
        if (!first) printf(" ");
        printf("%s", p->id);
        first = 0;
        p = p->next;
    }
    printf("\n");
}

void free_all(void) {
    Paragraph *p = head;
    while (p) {
        Paragraph *tmp = p;
        p = p->next;
        free(tmp->id);
        free(tmp->text);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char id[256];
        char text[512];
        if (sscanf(line, "%15s %255s", cmd, id) < 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *id_end = p;
            while (*id_end && *id_end != ' ') id_end++;
            if (*id_end) {
                *id_end = '\0';
                char *text_start = id_end + 1;
                while (*text_start == ' ') text_start++;
                add_paragraph(p, text_start);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id_end = p;
            while (*id_end && *id_end != ' ') id_end++;
            if (*id_end) {
                *id_end = '\0';
                char *text_start = id_end + 1;
                while (*text_start == ' ') text_start++;
                append_paragraph(p, text_start);
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *id_end = p;
            while (*id_end && *id_end != ' ') id_end++;
            if (*id_end) {
                *id_end = '\0';
                char *text_start = id_end + 1;
                while (*text_start == ' ') text_start++;
                replace_paragraph(p, text_start);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_paragraph(p);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            show_paragraph(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    free_all();
    return 0;
}