// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Paragraph {
    char *id;
    char *text;
    struct Paragraph *next;
} Paragraph;

Paragraph *head = NULL;

Paragraph *find_paragraph(const char *id) {
    Paragraph *cur = head;
    while (cur) {
        if (strcmp(cur->id, id) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_or_append(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) {
        p = malloc(sizeof(Paragraph));
        if (!p) return;
        p->id = safe_strdup(id);
        p->text = safe_strdup(text);
        p->next = head;
        head = p;
    } else {
        size_t old_len = strlen(p->text);
        size_t add_len = strlen(text);
        char *new_text = malloc(old_len + 1 + add_len + 1);
        if (!new_text) return;
        strcpy(new_text, p->text);
        new_text[old_len] = '\n';
        strcpy(new_text + old_len + 1, text);
        free(p->text);
        p->text = new_text;
    }
}

void replace_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (p) {
        free(p->text);
        p->text = safe_strdup(text);
    }
}

void delete_paragraph(const char *id) {
    Paragraph *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->id, id) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->id);
            free(cur->text);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void show_paragraph(const char *id) {
    Paragraph *p = find_paragraph(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (const char *c = p->text; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void list_paragraphs(void) {
    Paragraph *cur = head;
    while (cur) {
        printf("%s\n", cur->id);
        cur = cur->next;
    }
}

void free_all(void) {
    Paragraph *cur = head;
    while (cur) {
        Paragraph *next = cur->next;
        free(cur->id);
        free(cur->text);
        free(cur);
        cur = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char id[512];
        char text[MAX_LINE];
        int n = sscanf(line, "%31s %511s", cmd, id);
        if (n < 1) continue;

        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            if (n < 2) continue;
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            strncpy(text, p, sizeof(text) - 1);
            text[sizeof(text)-1] = '\0';

            if (strcmp(cmd, "NEW") == 0) {
                if (!find_paragraph(id)) {
                    Paragraph *pnew = malloc(sizeof(Paragraph));
                    if (pnew) {
                        pnew->id = safe_strdup(id);
                        pnew->text = safe_strdup(text);
                        pnew->next = head;
                        head = pnew;
                    }
                }
            } else if (strcmp(cmd, "APPEND") == 0) {
                add_or_append(id, text);
            } else if (strcmp(cmd, "REPLACE") == 0) {
                replace_paragraph(id, text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n >= 2) delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (n >= 2) show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    free_all();
    return 0;
}