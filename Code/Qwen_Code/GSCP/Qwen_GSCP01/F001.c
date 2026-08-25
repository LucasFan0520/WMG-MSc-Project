// F001.c
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
    Paragraph *curr = head;
    while (curr) {
        if (strcmp(curr->id, id) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_paragraph(const char *id, const char *text) {
    if (find_paragraph(id)) return;
    Paragraph *p = malloc(sizeof(Paragraph));
    if (!p) exit(EXIT_FAILURE);
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (!head) {
        head = p;
    } else {
        Paragraph *curr = head;
        while (curr->next) curr = curr->next;
        curr->next = p;
    }
}

void append_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) {
        add_paragraph(id, text);
        return;
    }
    size_t old_len = strlen(p->text);
    size_t add_len = strlen(text);
    char *new_text = malloc(old_len + add_len + 2);
    if (!new_text) exit(EXIT_FAILURE);
    strcpy(new_text, p->text);
    new_text[old_len] = ' ';
    strcpy(new_text + old_len + 1, text);
    free(p->text);
    p->text = new_text;
}

void replace_paragraph(const char *id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (!p) {
        add_paragraph(id, text);
        return;
    }
    free(p->text);
    p->text = strdup(text);
}

void delete_paragraph(const char *id) {
    Paragraph *curr = head;
    Paragraph *prev = NULL;
    while (curr) {
        if (strcmp(curr->id, id) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
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
    for (int i = 0; p->text[i]; i++) {
        if (p->text[i] == ' ') putchar('_');
        else putchar(p->text[i]);
    }
    putchar('\n');
}

void list_paragraphs() {
    Paragraph *curr = head;
    while (curr) {
        printf("%s\n", curr->id);
        curr = curr->next;
    }
}

void free_all() {
    Paragraph *curr = head;
    while (curr) {
        Paragraph *next = curr->next;
        free(curr->id);
        free(curr->text);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char id[256] = {0};
        char text[3800] = {0};
        
        int matched = sscanf(line, "%31s %255s %[^\n]", cmd, id, text);
        
        if (matched >= 2) {
            if (strcmp(cmd, "NEW") == 0 && matched == 3) add_paragraph(id, text);
            else if (strcmp(cmd, "APPEND") == 0 && matched == 3) append_paragraph(id, text);
            else if (strcmp(cmd, "REPLACE") == 0 && matched == 3) replace_paragraph(id, text);
            else if (strcmp(cmd, "DELETE") == 0) delete_paragraph(id);
            else if (strcmp(cmd, "SHOW") == 0) show_paragraph(id);
            else if (strcmp(cmd, "LIST") == 0) list_paragraphs();
        }
    }
    free_all();
    return 0;
}