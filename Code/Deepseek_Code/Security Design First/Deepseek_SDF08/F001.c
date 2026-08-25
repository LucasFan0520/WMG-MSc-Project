// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct para {
    char *id;
    char *text;
    struct para *next;
} Para;

Para *head = NULL;
Para *tail = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Para *find_para(const char *id) {
    Para *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void add_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (p) {
        free(p->text);
        p->text = strdup(text);
        if (!p->text) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    Para *newp = malloc(sizeof(Para));
    if (!newp) { fprintf(stderr, "Memory error\n"); exit(1); }
    newp->id = strdup(id);
    newp->text = strdup(text);
    if (!newp->id || !newp->text) { fprintf(stderr, "Memory error\n"); exit(1); }
    newp->next = NULL;
    if (tail) tail->next = newp;
    else head = newp;
    tail = newp;
}

void append_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (!p) return;
    size_t new_len = strlen(p->text) + strlen(text) + 1;
    char *new_text = realloc(p->text, new_len);
    if (!new_text) { fprintf(stderr, "Memory error\n"); exit(1); }
    p->text = new_text;
    strcat(p->text, text);
}

void replace_para(const char *id, const char *text) {
    Para *p = find_para(id);
    if (!p) return;
    free(p->text);
    p->text = strdup(text);
    if (!p->text) { fprintf(stderr, "Memory error\n"); exit(1); }
}

void delete_para(const char *id) {
    Para *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            if (p == tail) tail = prev;
            free(p->id);
            free(p->text);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void show_para(const char *id) {
    Para *p = find_para(id);
    if (!p) { printf("NOT_FOUND\n"); return; }
    for (char *s = p->text; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void list_paras(void) {
    Para *p = head;
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
    Para *p = head;
    while (p) {
        Para *next = p->next;
        free(p->id);
        free(p->text);
        free(p);
        p = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char id[256];
        char text[MAX_LINE];
        if (sscanf(line, "%31s %255s", cmd, id) < 1) continue;
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        char *id_start = rest;
        while (*rest && !isspace(*rest)) rest++;
        if (!*rest) continue;
        *rest = '\0';
        rest++;
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "NEW") == 0) {
            add_para(id_start, rest);
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_para(id_start, rest);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            replace_para(id_start, rest);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_para(id_start);
        } else if (strcmp(cmd, "SHOW") == 0) {
            show_para(id_start);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paras();
        }
    }
    free_all();
    return 0;
}