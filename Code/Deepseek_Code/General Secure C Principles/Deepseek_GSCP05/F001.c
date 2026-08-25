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

void add_paragraph(const char *id, const char *text) {
    if (find_paragraph(id)) return;
    Paragraph *p = malloc(sizeof(Paragraph));
    if (!p) return;
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

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) return;
    char *new_text = malloc(strlen(p->text) + strlen(text) + 1);
    if (!new_text) return;
    strcpy(new_text, p->text);
    strcat(new_text, text);
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
    for (char *s = p->text; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char id[256];
        char text[4096];
        if (sscanf(line, "%15s %255s", cmd, id) < 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *p = line + 4; // skip "NEW "
            while (*p == ' ') p++;
            char *id_start = p;
            char *id_end = strchr(id_start, ' ');
            if (!id_end) continue;
            *id_end = '\0';
            char *txt = id_end + 1;
            while (*txt == ' ') txt++;
            add_paragraph(id_start, txt);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id_start = p;
            char *id_end = strchr(id_start, ' ');
            if (!id_end) continue;
            *id_end = '\0';
            char *txt = id_end + 1;
            while (*txt == ' ') txt++;
            append_paragraph(id_start, txt);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *id_start = p;
            char *id_end = strchr(id_start, ' ');
            if (!id_end) continue;
            *id_end = '\0';
            char *txt = id_end + 1;
            while (*txt == ' ') txt++;
            replace_paragraph(id_start, txt);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id_start = p;
            char *id_end = strchr(id_start, ' ');
            if (id_end) *id_end = '\0';
            delete_paragraph(id_start);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *id_start = p;
            char *id_end = strchr(id_start, ' ');
            if (id_end) *id_end = '\0';
            show_paragraph(id_start);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    free_all();
    return 0;
}