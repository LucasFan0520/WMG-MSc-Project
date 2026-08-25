// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct paragraph {
    char *id;
    char *text;
    struct paragraph *next;
};

struct paragraph *head = NULL;
struct paragraph *tail = NULL;

void add_paragraph(char *id, char *text) {
    struct paragraph *p = malloc(sizeof(struct paragraph));
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

struct paragraph *find_paragraph(char *id) {
    struct paragraph *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void delete_paragraph(char *id) {
    struct paragraph *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (tail == p) tail = prev;
            free(p->id);
            free(p->text);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void show_paragraph(char *id) {
    struct paragraph *p = find_paragraph(id);
    if (p) {
        for (char *c = p->text; *c; c++) {
            if (*c == ' ') putchar('_');
            else putchar(*c);
        }
        putchar('\n');
    } else {
        printf("NOT_FOUND\n");
    }
}

void list_paragraphs() {
    struct paragraph *p = head;
    int first = 1;
    while (p) {
        if (!first) printf(" ");
        printf("%s", p->id);
        first = 0;
        p = p->next;
    }
    printf("\n");
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256];
        char text[4000];
        if (sscanf(line, "%31s %255s", cmd, id) < 2) continue;
        char *rest = line + strlen(cmd) + 1 + strlen(id) + 1;
        if (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0) {
            if (find_paragraph(id)) continue;
            add_paragraph(id, rest);
        } else if (strcmp(cmd, "APPEND") == 0) {
            struct paragraph *p = find_paragraph(id);
            if (p) {
                char *newtext = malloc(strlen(p->text) + strlen(rest) + 1);
                strcpy(newtext, p->text);
                strcat(newtext, rest);
                free(p->text);
                p->text = newtext;
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            struct paragraph *p = find_paragraph(id);
            if (p) {
                free(p->text);
                p->text = malloc(strlen(rest) + 1);
                strcpy(p->text, rest);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    while (head) {
        struct paragraph *p = head;
        head = head->next;
        free(p->id);
        free(p->text);
        free(p);
    }
    return 0;
}