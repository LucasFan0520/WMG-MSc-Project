// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Paragraph {
    int id;
    char *text;
    struct Paragraph *next;
} Paragraph;

Paragraph *head = NULL;

Paragraph *find_paragraph(int id) {
    Paragraph *curr = head;
    while (curr) {
        if (curr->id == id) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_paragraph(int id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (p) {
        size_t new_len = strlen(p->text) + strlen(text) + 2;
        char *new_text = malloc(new_len);
        if (!new_text) return;
        snprintf(new_text, new_len, "%s %s", p->text, text);
        free(p->text);
        p->text = new_text;
    } else {
        p = malloc(sizeof(Paragraph));
        if (!p) return;
        p->id = id;
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
}

void replace_paragraph(int id, const char *text) {
    Paragraph *p = find_paragraph(id);
    if (p) {
        free(p->text);
        p->text = strdup(text);
    }
}

void delete_paragraph(int id) {
    Paragraph *curr = head;
    Paragraph *prev = NULL;
    while (curr) {
        if (curr->id == id) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void show_paragraph(int id) {
    Paragraph *p = find_paragraph(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (size_t i = 0; i < strlen(p->text); i++) {
        if (p->text[i] == ' ') putchar('_');
        else putchar(p->text[i]);
    }
    putchar('\n');
}

void list_paragraphs() {
    Paragraph *curr = head;
    while (curr) {
        printf("%d\n", curr->id);
        curr = curr->next;
    }
}

void free_all() {
    Paragraph *curr = head;
    while (curr) {
        Paragraph *next = curr->next;
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
        
        char cmd[16];
        int id;
        char text[4096] = {0};
        
        int n = sscanf(line, "%15s %d %[^\n]", cmd, &id, text);
        if (n >= 2) {
            if (strcmp(cmd, "NEW") == 0 && n == 3) add_paragraph(id, text);
            else if (strcmp(cmd, "APPEND") == 0 && n == 3) add_paragraph(id, text);
            else if (strcmp(cmd, "REPLACE") == 0 && n == 3) replace_paragraph(id, text);
            else if (strcmp(cmd, "DELETE") == 0) delete_paragraph(id);
            else if (strcmp(cmd, "SHOW") == 0) show_paragraph(id);
        } else if (sscanf(line, "%15s", cmd) == 1) {
            if (strcmp(cmd, "LIST") == 0) list_paragraphs();
        }
    }
    free_all();
    return 0;
}