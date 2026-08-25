// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

void print_underscores(const char *s) {
    while (*s) {
        putchar(*s == ' ' ? '_' : *s);
        s++;
    }
    putchar('\n');
}

typedef struct Paragraph {
    char *id;
    char *text;
    struct Paragraph *next;
} Paragraph;

Paragraph *head = NULL;
Paragraph *tail = NULL;

Paragraph *find_paragraph(const char *id) {
    for (Paragraph *p = head; p; p = p->next) {
        if (strcmp(p->id, id) == 0) return p;
    }
    return NULL;
}

void add_paragraph(const char *id, const char *text) {
    Paragraph *p = malloc(sizeof(Paragraph));
    p->id = strdup(id);
    p->text = strdup(text);
    p->next = NULL;
    if (tail) tail->next = p;
    else head = p;
    tail = p;
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

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, id[256] = {0};
        sscanf(line, "%15s %255s", cmd, id);
        char *text = strchr(line, ' ');
        if (text) {
            text++;
            while (*text == ' ') text++;
        }
        
        if (strcmp(cmd, "NEW") == 0 && text) {
            add_paragraph(id, text);
        } else if (strcmp(cmd, "APPEND") == 0 && text) {
            Paragraph *p = find_paragraph(id);
            if (p) {
                size_t len = strlen(p->text) + 1 + strlen(text) + 1;
                char *new_text = malloc(len);
                snprintf(new_text, len, "%s %s", p->text, text);
                free(p->text);
                p->text = new_text;
            }
        } else if (strcmp(cmd, "REPLACE") == 0 && text) {
            Paragraph *p = find_paragraph(id);
            if (p) {
                free(p->text);
                p->text = strdup(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            Paragraph *p = find_paragraph(id);
            if (p) print_underscores(p->text);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Paragraph *p = head; p; p = p->next) {
                printf("%s\n", p->id);
            }
        }
        free(line);
    }
    
    Paragraph *p = head;
    while (p) {
        Paragraph *next = p->next;
        free(p->id);
        free(p->text);
        free(p);
        p = next;
    }
    return 0;
}