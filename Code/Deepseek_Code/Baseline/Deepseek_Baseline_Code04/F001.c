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

void add_paragraph(const char *id, const char *text) {
    Paragraph *p = malloc(sizeof(Paragraph));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (!head) {
        head = p;
    } else {
        Paragraph *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = p;
    }
}

void free_paragraphs(void) {
    while (head) {
        Paragraph *tmp = head;
        head = head->next;
        free(tmp->id);
        free(tmp->text);
        free(tmp);
    }
}

char *replace_spaces(const char *s) {
    char *r = strdup(s);
    for (char *p = r; *p; p++) if (*p == ' ') *p = '_';
    return r;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char id[256];
        char text[4096];
        int n = sscanf(line, "%s %s", cmd, id);
        if (n < 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *rest = line + strlen(cmd) + 1 + strlen(id) + 1;
            if (*rest) {
                add_paragraph(id, rest);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (n < 2) continue;
            Paragraph *p = find_paragraph(id);
            if (!p) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(id) + 1;
            if (*rest) {
                char *newtext = malloc(strlen(p->text) + strlen(rest) + 1);
                strcpy(newtext, p->text);
                strcat(newtext, rest);
                free(p->text);
                p->text = newtext;
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            if (n < 2) continue;
            Paragraph *p = find_paragraph(id);
            if (!p) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(id) + 1;
            if (*rest) {
                free(p->text);
                p->text = strdup(rest);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            Paragraph *prev = NULL, *cur = head;
            while (cur) {
                if (strcmp(cur->id, id) == 0) {
                    if (prev) prev->next = cur->next;
                    else head = cur->next;
                    free(cur->id);
                    free(cur->text);
                    free(cur);
                    break;
                }
                prev = cur;
                cur = cur->next;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            if (n < 2) continue;
            Paragraph *p = find_paragraph(id);
            if (p) {
                char *s = replace_spaces(p->text);
                printf("%s\n", s);
                free(s);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            Paragraph *p = head;
            while (p) {
                printf("%s\n", p->id);
                p = p->next;
            }
        }
    }
    free_paragraphs();
    return 0;
}